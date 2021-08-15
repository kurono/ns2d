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

class Array2D {
    constructor(rows, cols, defaultValue = 0.0) {
        this.rows = rows;
        this.cols = cols;
        this.#fillData(defaultValue);
    }

    // private fields and methods start from #
    // It is need to use object instead of arrays
    // in order to be able to be passed by reference
    #fillData(defaultValue) {
        let mx = [];

        // loop over rows
        for (let r = 0; r < this.rows; r++) {
            // loop over cols
            let row = {};
            for (let c = 0; c < this.cols; c++) {
                row[c] = defaultValue; // append a value
            }
            this[r] = row;
        }

        return mx;
    }

    print() {
        let all = "";
        for (let r = 0; r < this.rows; r++) {
            let row = "";
            for (let c = 0; c < this.cols; c++) {
                row += `${this[r][c]}\t`;
            }
            all += `${row}\n`;
        }
        console.log(all);
    }

    get min() {
        let v = this[0][0];

        for (let iy = 0; iy < this.rows; iy++) {
            for (let ix = 0; ix < this.cols; ix++) {
                v = (this[iy][ix] < v) ? this[iy][ix] : v;
            }
        }

        return v;
    }

    get max() {
        let v = this[0][0];

        for (let iy = 0; iy < this.rows; iy++) {
            for (let ix = 0; ix < this.cols; ix++) {
                v = (this[iy][ix] > v) ? this[iy][ix] : v;
            }
        }

        return v;
    }

}