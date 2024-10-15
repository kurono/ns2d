# ns2d
A simple 2D incompressible Navier-Stokes solver on a regular Cartesian quadrilateral grid, utilizing the Chorin projection/decomposition method.

### How to solve the incompressible Navier-Stokes equations:
The governing equations are:
$$
\frac{\partial \mathbf{u}}{\partial t} = -(\mathbf{u} \cdot \nabla) \mathbf{u} - \frac{1}{\rho} \nabla p + \nu \nabla^2 \mathbf{u}
$$
$$
\nabla \cdot \mathbf{u} = 0
$$

We use the splitting method based on the Helmholtz-Hodge decomposition. For any arbitrary vector field $\mathbf{w}$, we decompose it as:
$$
\mathbf{w} = \mathbf{u} + \nabla \phi
$$
with:
$$
\nabla \cdot \mathbf{u} = 0, \quad \nabla \times \mathbf{u} \neq 0
$$
$$
\nabla \cdot (\nabla \phi) \neq 0, \quad \nabla \times (\nabla \phi) = 0
$$

Thus, the velocity field becomes:
$$
\mathbf{w} = \mathbf{u} + \frac{\Delta t}{\rho} \nabla p
$$

### Steps of time-explicit method:

#### 1. Diffusion step: 
$$
\frac{\partial \mathbf{u}}{\partial t} = \nu \nabla^2 \mathbf{u}
$$
**Finite Difference (FD) form**: 
$$
\mathbf{u} - \mathbf{u}_{\text{prev}} = \Delta t \cdot \nu \nabla^2 \mathbf{u}_{\text{prev}}
$$

#### 2. Advection step: 
$$
\frac{\partial \mathbf{u}}{\partial t} = - (\mathbf{u} \cdot \nabla) \mathbf{u}
$$
**FD form**: 
$$
\mathbf{u} - \mathbf{u}_{\text{prev}} = \Delta t \cdot \left( - (\mathbf{u} \cdot \nabla) \mathbf{u} \right)
$$

#### 3. Poisson equation: 
$$
\nabla^2 p = \frac{\rho}{\Delta t} \nabla \cdot \mathbf{w}
$$
**FD form**:
$$
-4p(i_y,i_x) + p(i_y+1,i_x) + p(i_y-1,i_x) + p(i_y,i_x+1) + p(i_y,i_x-1) = h^2 \cdot \frac{\rho}{\Delta t} \nabla \cdot \mathbf{u} = S
$$
$$
p(i_y,i_x) = \frac{1}{4} \left( p(i_y+1,i_x) + p(i_y-1,i_x) + p(i_y,i_x+1) + p(i_y,i_x-1) - S \right)
$$

#### 4. Apply pressure: 
$$
\mathbf{u} = \mathbf{u}_{\text{prev}} - \Delta t \cdot \frac{1}{\rho} \nabla p
$$

---

### Demo

A test case involves lid-driven flow in a box cavity. All walls have zero Dirichlet velocity conditions, except for the top side, where a sliding velocity is defined. This velocity periodically changes its direction from left to right and vice versa. The magnitude of the velocity field is represented using ASCII art.

![Demo](./assets/demo_ns_anim.mp4)
