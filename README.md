# ns2d
Simple 2D incompressible Navier-Stokes solver on reguar Cartesian quad grid

###How to solve incompressible Navier-Stokes equations:
> du/dt = -(u * nabla)u - (1/rho) * gradP + nu * laplacian(u)

> div(u) = 0

We use splitting method
based on the Helmoltz-Hodge decomposition:
For any arbitrary vector field w:
> w = u + grad(phi)
div(u) = 0, rot(u) /= 0 ('/=' means 'not equal')
div(grad(phi)) /= 0, rot(grad(phi)) === 0

So:
> w = u + (dt/rho) * grad(p)

#### 1: diffuse: du/dt = nu*laplacian(u)
**FD**: u-uprev = dt*(nu*laplacian(uprev))

#### 2: advect: du/dt = -(u*nabla)u
**FD**: u - uprev = dt*( -(u*nabla)u )

#### 3: Poisson eqn: laplacian(P) = (rho/dt) * div(w)
**FD**: - 4*P(iy,ix) + P(iy+1,ix) + P(iy-1,ix) + P(iy,ix+1) + P(iy,ix-1) = (h*h) * (rho/dt) * div(w) = S
> P(iy,ix) = (1/4)*(P(iy+1,ix) + P(iy-1,ix) + P(iy,ix+1) + P(iy,ix-1) - S)

#### 4: apply pressure
**FD**: u - uprev = dt*( - 1/rho*gradP );
