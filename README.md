# ns2d
A simple 2D incompressible Navier-Stokes solver on a regular Cartesian quadrilateral grid.

### How to solve the incompressible Navier-Stokes equations:
The governing equations are:
\[
\frac{\partial \mathbf{u}}{\partial t} = -(\mathbf{u} \cdot \nabla) \mathbf{u} - \frac{1}{\rho} \nabla P + \nu \nabla^2 \mathbf{u}
\]
\[
\nabla \cdot \mathbf{u} = 0
\]

We use the splitting method based on the Helmholtz-Hodge decomposition. For any arbitrary vector field $\mathbf{w}$, we decompose it as:
\[
\mathbf{w} = \mathbf{u} + \nabla \phi
\]
with:
\[
\nabla \cdot \mathbf{u} = 0, \quad \nabla \times \mathbf{u} \neq 0
\]
\[
\nabla \cdot (\nabla \phi) \neq 0, \quad \nabla \times (\nabla \phi) = 0
\]

Thus, the velocity field becomes:
\[
\mathbf{w} = \mathbf{u} + \frac{\Delta t}{\rho} \nabla p
\]

### Steps of the method:

#### 1. Diffusion step: 
\[
\frac{\partial \mathbf{u}}{\partial t} = \nu \nabla^2 \mathbf{u}
\]
**Finite Difference (FD) form**: 
\[
\mathbf{u} - \mathbf{u}_{\text{prev}} = \Delta t \cdot \nu \nabla^2 \mathbf{u}_{\text{prev}}
\]

#### 2. Advection step: 
\[
\frac{\partial \mathbf{u}}{\partial t} = - (\mathbf{u} \cdot \nabla) \mathbf{u}
\]
**FD form**: 
\[
\mathbf{u} - \mathbf{u}_{\text{prev}} = \Delta t \cdot \left( - (\mathbf{u} \cdot \nabla) \mathbf{u} \right)
\]

#### 3. Poisson equation: 
\[
\nabla^2 P = \frac{\rho}{\Delta t} \nabla \cdot \mathbf{w}
\]
**FD form**:
\[
-4P(i_y,i_x) + P(i_y+1,i_x) + P(i_y-1,i_x) + P(i_y,i_x+1) + P(i_y,i_x-1) = h^2 \cdot \frac{\rho}{\Delta t} \nabla \cdot \mathbf{w} = S
\]
\[
P(i_y,i_x) = \frac{1}{4} \left( P(i_y+1,i_x) + P(i_y-1,i_x) + P(i_y,i_x+1) + P(i_y,i_x-1) - S \right)
\]

#### 4. Apply pressure: 
\[
\mathbf{u} - \mathbf{u}_{\text{prev}} = \Delta t \cdot \left( - \frac{1}{\rho} \nabla P \right)
\]

---

### Demo
![Demo](./assets/ns_ascii.gif)
