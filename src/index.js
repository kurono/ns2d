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

let app;
let frame = 0;
const w = 5e2; // velocity direction change frequency [1/s]

/**
 * Main entry point
 * @constructor
 */
function Main() {
    // Ctrl + Alt + L -> auto-format

    app = new Program();

    console.log("Start");

    // init u, v, p
    app.init_fields();

    // start the simulation
    onEnterFrame(app);
}

/**
 * A callback to update the screen every frame
 * @param app
 */
function onEnterFrame(app) {
    requestAnimationFrame(function() { onEnterFrame(app); });

    // time loop
    if (app._t < app._tmax)
    {
        //if (frame === 0)
        //{
            app.clear_console();

            app.compute_umag();

            app.draw_field(app._umag, "u [m/s]");
        //}

        let u_posy = app._umax * Math.cos(w * app._t);

        app.solve(u_posy);

        // next step
        app._t += app._dt;

        /*frame++;
        if (frame > app._skip_frames)
        {
            frame = 0;
        }*/
    } else {
        app.WriteLine("End");
    }
}