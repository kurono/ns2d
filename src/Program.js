/**
 * Copyright 2021 Ilya Tsivilskiy @Kei https://github.com/kurono

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

 http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
 */

class Program {
    constructor() {
        console.log("Program initialized");

        this.EPS = 1.0e-9;

        // console-printed sceen size
        this._aspectRatio = 2;
        this._conH = 20;
        this._conW = this._conH * this._aspectRatio;

        // vertex count in each direction
        this._res = 40;

        // domain size
        this._dsize = 0.1;

        // mesh cell size
        this._h = this._dsize / (this._res - 1.0);

        // time stepping
        this._tmax = 10.0; // [s]
        this._dt = 1.0e-5; // [s]
        this._maxit = 20; // Gauss-Seidel iterations

        // define material properties
        this._rho = 1000.0; // [kg/m3]
        this._mu = 100.0; // [Pa*s]
        this._nu = this._mu / this._rho;

        // fields
        this._u = [];
        this._v = [];
        this._p = [];
        this._umag = [];
        this._uprev = [];
        this._vprev = [];

        // time
        this._t = 0.0;

        // velocity at the boundary
        this._umax = 0.01;

        this._skip_frames = 10;

        /*
        let a = this.new_matrix(this._res, this._res);
        a[0][0] = 1;
        a[3][2] = 3;
        this.print_matrix(a);

        let b = this.new_matrix(this._res, this._res);
        this.current_to_prev(a, b);
        this.print_matrix(b);
        */

        this._console = document.getElementById("consoleDiv");

        /*
        let a = new Array2D(this._res, this._res);
        a[0][0] = 1;
        a[3][2] = 3;
        a.print();

        console.log(a);

        let b = new Array2D(this._res, this._res);
        this.current_to_prev(a, b);
        b.print();
        */
    }

    /*
    new_matrix(nrows, ncols, default_value = 0.0) {
        let mx = [];

        // loop over rows
        for (let r = 0; r < nrows; r++) {
            // loop over cols
            let row = [];
            for (let c = 0; c < ncols; c++) {
                row.push(default_value); // append a value
            }
            mx.push(row);
        }

        return mx;
    }

    print_matrix(mx) {
        let all = "";
        for (let r = 0; r < mx.length; r++) {
            let row = "";
            for (let c = 0; c < mx[0].length; c++) {
                row += `${mx[r][c]}\t`;
            }
            all += `${row}\n`;
        }
        console.log(all);
    }
    */

    init_fields()
    {
        this._u = new Array2D(this._res, this._res);
        this._v = new Array2D(this._res, this._res);
        this._p = new Array2D(this._res, this._res);

        this._uprev = new Array2D(this._res, this._res);
        this._vprev = new Array2D(this._res, this._res);

        this._umag = new Array2D(this._res, this._res);
    }

    /**
     * Compute magnitude of 2D velocity field
     */
    compute_umag()
    {
        let u, v;

        for (let iy = 0; iy < this._res; iy++)
        {
            for (let ix = 0; ix < this._res; ix++)
            {
                u = this._u[iy][ix];
                v = this._v[iy][ix];
                this._umag[iy][ix] = Math.sqrt(u * u + v * v);
            }
        }
    }

    /**
     * Copy data from 'f' to 'fprev', pass args by reference
     * @param f an Array2D to get data from
     * @param fprev an Array2D to set data to
     */
    current_to_prev(f, fprev) {
        // WARN: can pass by reference if to use objects as arguments (not arrays or primitive types)
        for (let iy = 0; iy < this._res; iy++) {
            for (let ix = 0; ix < this._res; ix++) {
                fprev[iy][ix] = f[iy][ix];
            }
        }
    }

    /**
     * Explicitly solve for diffuse equation on field 'f' passed by reference
     * @param f an Array2D representing the scalar field defined on regular cartesian grid
     */
    diffuse(f) {
        let d2fdx2, d2fdy2, lapf;
        let hh = this._h * this._h;

        for (let iy = 1; iy < (this._res - 1); iy++) {
            for (let ix = 1; ix < (this._res - 1); ix++) {
                d2fdx2 = (f[iy][ix + 1] + f[iy][ix - 1] - 2.0 * f[iy][ix]) / hh;
                d2fdy2 = (f[iy + 1][ix] + f[iy - 1][ix] - 2.0 * f[iy][ix]) / hh;
                lapf = d2fdx2 + d2fdy2;
                f[iy][ix] += this._dt * this._nu * lapf;
            }
        }
    }

    /**
     * Explicitly advect the field 'f' through the vector field defined by two components 'u' and 'v'
     * @param f an Array2D to advect over the vector field
     * @param u x-component of the vector field
     * @param v y-component of the vector field
     */
    advect(f, u, v) {
        let dfdx, dfdy, unablaf;
        let h2 = this._h * 2.0;

        for (let iy = 1; iy < (this._res - 1); iy++) {
            for (let ix = 1; ix < (this._res - 1); ix++) {
                dfdx = (f[iy][ix + 1] - f[iy][ix - 1]) / h2;
                dfdy = (f[iy + 1][ix] - f[iy - 1][ix]) / h2;
                unablaf = u[iy][ix] * dfdx + v[iy][ix] * dfdy;
                f[iy][ix] -= this._dt * unablaf;
            }
        }
    }

    /**
     * Set the Dirichlet-type boundary condition
     * @param f
     * @param negx
     * @param posx
     * @param negy
     * @param posy
     */
    bound_dirichlet(f, negx, posx, negy, posy) {
        let iy, ix;

        // negy
        iy = 0;
        for (ix = 0; ix < this._res; ix++) {
            f[iy][ix] = negy;
        }

        // posy
        iy = this._res - 1;
        for (ix = 0; ix < this._res; ix++) {
            f[iy][ix] = posy;
        }

        // negx
        ix = 0;
        for (iy = 0; iy < this._res; iy++) {
            f[iy][ix] = negx;
        }

        // posx
        ix = this._res - 1;
        for (iy = 0; iy < this._res; iy++) {
            f[iy][ix] = posx;
        }
    }

    /**
     * Set Neumann-type boundary conditions (zero normal gradient)
     * @param f
     */
    bound_neumann(f) {
        let iy, ix;

        // negy
        iy = 0;
        for (ix = 0; ix < this._res; ix++) {
            f[iy][ix] = f[iy + 1][ix];
        }

        // posy
        iy = this._res - 1;
        for (ix = 0; ix < this._res; ix++) {
            f[iy][ix] = f[iy - 1][ix];
        }

        // negx
        ix = 0;
        for (iy = 0; iy < this._res; iy++) {
            f[iy][ix] = f[iy][ix + 1];
        }

        // posx
        ix = this._res - 1;
        for (iy = 0; iy < this._res; iy++) {
            f[iy][ix] = f[iy][ix - 1];
        }
    }

    /**
     * Implicitly solve the Poisson's equation to compute field 'p' auto-applying Neumann's BCs
     * @param u
     * @param v
     * @param p
     */
    implicit_poisson(u, v, p) {
        let hh = this._h * this._h;
        let h2 = this._h * 2.0;
        let dudx, dvdy, divu;
        let src, sum4;

        // Gauss-Seidel loop
        for (let it = 0; it < this._maxit; it++) {
            for (let iy = 1; iy < (this._res - 1); iy++) {
                for (let ix = 1; ix < (this._res - 1); ix++) {
                    // compute velocity divergence
                    dudx = (u[iy][ix + 1] - u[iy][ix - 1]) / h2;
                    dvdy = (v[iy + 1][ix] - v[iy - 1][ix]) / h2;
                    divu = dudx + dvdy;

                    // source term in the Poisson eqn
                    src = (this._rho / this._dt) * divu;

                    // implicit integration
                    sum4 = p[iy + 1][ix] + p[iy - 1][ix] + p[iy][ix + 1] + p[iy][ix - 1];
                    p[iy][ix] = 0.25 * (sum4 - hh * src);
                }
            }

            this.bound_neumann(p);
        }
    }

    /**
     * Explicitly apply the pressure 'p' gradient to the velocity field ('u', 'v')
     * @param u
     * @param v
     * @param p
     */
    apply_gradp(u, v, p) {
        let dpdx, dpdy;
        let h2 = this._h * 2.0;

        for (let iy = 1; iy < (this._res - 1); iy++) {
            for (let ix = 1; ix < (this._res - 1); ix++) {
                // compute pressure gradient
                dpdx = (p[iy][ix + 1] - p[iy][ix - 1]) / h2;
                dpdy = (p[iy + 1][ix] - p[iy - 1][ix]) / h2;

                // explicit integration
                u[iy][ix] -= this._dt * (dpdx / this._rho);
                v[iy][ix] -= this._dt * (dpdy / this._rho);
            }
        }
    }

    /**
     * Solve for incompressible Navier-Stokes equation discretized via the finite difference method
     * @param umax Velocity BC to set on the 'negy' boundary
     */
    solve(umax) {
        // 1: advect velocity through itself
        this.advect(this._u, this._uprev, this._vprev);
        this.advect(this._v, this._uprev, this._vprev);

        // set velocity at the boundary
        this.bound_dirichlet(this._u, 0.0, 0.0, 0.0, umax);
        this.bound_dirichlet(this._v, 0.0, 0.0, 0.0, 0.0);

        // 2: diffuse velocity
        this.diffuse(this._u);
        this.diffuse(this._v);

        // set velocity at the boundary
        this.bound_dirichlet(this._u, 0.0, 0.0, 0.0, umax);
        this.bound_dirichlet(this._v, 0.0, 0.0, 0.0, 0.0);

        // 3: solve for pressure
        this.implicit_poisson(this._u, this._v, this._p);

        // 4: apply pressure
        this.apply_gradp(this._u, this._v, this._p);

        // set velocity at the boundary
        this.bound_dirichlet(this._u, 0.0, 0.0, 0.0, umax);
        this.bound_dirichlet(this._v, 0.0, 0.0, 0.0, 0.0);

        // update prev
        this.current_to_prev(this._u, this._uprev);
        this.current_to_prev(this._v, this._vprev);
    }

    /**
     * Clear the text in the '_console' div element
     */
    clear_console()
    {
        this._console.innerHTML = "";
    }

    /**
     * ASCII-art-style visualization of the 'f' of Array2D in the '_console' div element
     * @param f
     * @param title
     */
    draw_field(f, title) {
        let iy, ix; // indices in terms of the 'f' field
        let nx, ny; // normalized screen positions

        // compute field limits
        let fmin = f.min;
        let fmax = f.max;

        let fn;

        // colormap in ascending order
        let cmap = ["\u2591", "\u2592", "\u2593", "\u2588"]; // unicode

        let cmapSize = cmap.length - 1;

        let ic; // index in a color map

        let icb = cmapSize; // key color index in a color bar
        let odd_line = false;

        let all = `t = ${this._t.toExponential(3)} [s]\n\n`;

        // 2D loop over console symbol positions
        for (let py = 0; py < this._conH; py++) {
            let row = "";
            for (let px = 0; px < this._conW; px++) {
                // normalize screen coordinates
                ny = (((this._conH - 1.0) - py) / (this._conH - 1.0));
                nx = (px / (this._conW - 1.0));

                // convert normalized screen point to field index
                iy = Math.round((this._res - 1.0) * ny);
                ix = Math.round((this._res - 1.0) * nx);

                // compute normalized field data
                fn = (f[iy][ix] - fmin) / (fmax - fmin + this.EPS);

                // map field data to color space
                ic = Math.round(cmapSize * fn);

                // draw the symbol
                row += `${cmap[ic]}`;
            }

            // draw color bar
            if (odd_line) {
                if (icb >= 0) {
                    let dataColor = cmap[icb];
                    let dataValue = (fmin + (fmax - fmin) *  icb / (cmapSize - 1.0)).toExponential(3);
                    row += ` |${dataColor}| ${dataValue}`;
                } else {
                    row += " | |\t";
                }
                icb--;
            } else {
                if (py === 0) {
                    row += ` ${title}`;
                } else {
                    row += " | |\t";
                }
            }

            odd_line = !odd_line;

            all += `${row}\n`;
        }

        this.draw_to_console(all);
    }

    draw_to_console(text) {
        this._console.innerHTML = text;
    }

}