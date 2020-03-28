#!/usr/bin/env node
/*
 * Copyright (c) 2019 Yahweasel
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

const fs = require("fs");
const inp = fs.readFileSync(process.argv[2], "utf8").split("\n");
const outp = fs.createWriteStream(process.argv[3], "utf8");

function hex(x) {
    return Number.parseInt(x, 16);
}

// General info about this world
var chr = "ff3a";
var palette = []
var defs = {};
var sprites = [];
var world, w, h;

// Default palette is all black
(function() {
    for (var i = 0; i < 0x10; i++)
        palette.push(0x0f);
})();

// First handle all definitions
(function() {
    for (var i = 0; i < inp.length; i++) {
        var line = inp[i].trim().split(/\s+/g);
        if (line[0] === "def" && line.length >= 4) {
            // Symbol being defined
            var sym = line[1];
            if (sym === "space")
                sym = " ";

            // Associated tile
            var def = hex(line[2]) ^ 0x80;
            if (def & 0x3) {
                console.error("Tile must always be divisible by 4: " + def.toString(16));
                process.exit(1);
            }

            // Associated palette
            def |= hex(line[3]) & 0x3;

            // Attributes
            var attr = 0;
            if (!line[4] || !+line[4])
                attr |= 0x1;

            // Action
            if (line[5])
                attr |= 0x2;
            var act = null;
            if (line[5])
                act = line[5];

            defs[sym] = {def, attr, act};

        } else if (line[0] === "chr" && line[1]) {
            chr = line[1];

        } else if (line[0] === "palette") {
            // Color palette for background tiles
            for (var j = 0; line[1+j]; j++)
                palette[j] = hex(line[1+j]);

        } else if (line[0] === "sprite" && line.length >= 5) {
            // A sprite to load into the world
            (function() {
                var x = hex(line[1]);
                var y = hex(line[2]);
                var spr = line[3];
                var stat = hex(line[4]);
                var act = null;
                if (line.length >= 6)
                    act = line[5];
                sprites.push({x, y, spr, stat, act});
            })();

        } else if (line[0] === "world") {
            break;

        } else if ((line[0] === "" && line.length === 1) ||
                   line[0][0] === "#") {
            // Comments

        } else {
            console.error("Unrecognized line " + inp[i]);
            process.exit(1);

        }
    }
})();

// Get to just the world itself
(function() {
    for (var i = 0; i < inp.length; i++) {
        var line = inp[i].trim();
        if (line === "world")
            world = inp.slice(i+1, inp.length - 1);
    }
})();

if (!world) {
    console.error("No world tile definition!");
    process.exit(1);
}

// Figure out the width and height of the world
w = world[0].length / 16;
h = world.length / 15;
if (~~w !== w || ~~h !== h) {
    console.error("World must be divisible into 16x15 screens.");
    process.exit(1);
}

// Write out the header
outp.write(".byte " + w + "," + h + "," + palette.join(",") + ",chr_" + chr + "_bank,chr_" + chr);

// Then make the actual world
(function() {
    for (var y = 0; y < world.length; y += 15) {
        for (var x = 0; x < world[0].length; x += 16) {
            // x,y now points to the upper-left corner of a screen to output
            for (var ys = 0; ys < 15; ys++) {
                for (var xs = 0; xs < 16; xs++) {
                    var c = world[y+ys][x+xs];
                    if (!(c in defs)) {
                        console.error("Undefined symbol " + c + "!");
                        process.exit(1);
                    }
                    outp.write("," + defs[c].def);
                }
            }
        }
    }
})();

// Then the attributes
(function() {
    for (var y = 0; y < world.length; y += 15) {
        for (var x = 0; x < world[0].length; x += 16) {
            for (var ys = 0; ys < 15; ys++) {
                for (var xs = 0; xs < 16; xs += 4) {
                    var row = world[y+ys];
                    var v =
                         defs[row[x+xs]].attr +
                        (defs[row[x+xs+1]].attr<<2) +
                        (defs[row[x+xs+2]].attr<<4) +
                        (defs[row[x+xs+3]].attr<<6);
                    outp.write("," + v);
                }
            }
        }
    }
})();

// Then the sprites
(function() {
    var i;
    for (i = 0; i < sprites.length && i < 15; i++) {
        var s = sprites[i];
        var xs = ~~(s.x/16);
        var ys = ~~(s.y/15);
        var xt = s.x%16;
        var yt = s.y%15;
        outp.write("," + xs + "," + ys + "," + xt + "," + yt +
                   ",spr_" + s.spr + "," + s.stat);
        if (s.act)
            outp.write(",act_" + s.act + "_bank,act_" + s.act);
        else
            outp.write(",0,0");
    }
    for (; i < 15; i++)
        outp.write(",255,0,0,0,0,0,0,0");
})();

// Then the actions
(function() {
    for (var y = 0; y < world.length; y += 15) {
        for (var x = 0; x < world[0].length; x += 16) {
            for (var ys = 0; ys < 15; ys++) {
                for (var xs = 0; xs < 16; xs++) {
                    var def = defs[world[y+ys][x+xs]];
                    if (def.act) {
                        // This tile has an action
                        outp.write(",act_" + def.act + "_bank,act_" + def.act +
                                   "," + (x/16) + "," + (y/15) +
                                   "," + xs + "," + ys);
                    }
                }
            }
        }
    }

    // Action with bank = 0 is end of actions, and end of file
    outp.write(",0\n");
    outp.end();
})();
