// Constants
import * as PIXI from 'pixi.js';

const TILE_EMPTY = -1;
const TILE_CLEARED = 0;
const TILE_START = 1;
const TILE_SIZE = 50; // pixels
const NUM_TYPES = 7; // colors
const BLOCK_SIZE = 2;
const FIELD_WIDTH = 6;
const FIELD_HEIGHT = 12;
const CLEAR_MIN = 4;
const POINTS_PER_CLEAR = 4;
const HAZARD_DROP_PER_CLEAR = 5;
const HAZARD_INTERVAL = 20;

// Orientations
const NORTH = 0;
const EAST = 1;
const SOUTH = 2;
const WEST = 3;

// States
const BLOCK_RELEASE = 0;
const BLOCK_FALLING = 1;
const FORM_CONNECTIONS = 2;
const CLEAR_SETS = 3;
const WIPE_CLEARED = 4;
const HAZARD_CHECK = 5;
const GAME_OVER = 6;

// Keys
const KEY_DOWN = 0;
const KEY_LEFT = 1;
const KEY_RIGHT = 2;
const KEY_ROTATE_LEFT = 3;
const KEY_ROTATE_RIGHT = 4;
const KEY_DROP = 5;
const NUM_KEYS = 6;

const TILE_NAMES = ['bar', 'bell', 'cherry', 'diamond', 'lemon', 'goldbars', 'seven'];

const FONT_STYLE = { fontFamily: 'Desyrel', fontSize: 36 };
const FONT_STYLE_LARGE = { fontFamily: 'Desyrel', fontSize: 64 };
const BUTTON_OPTS = { repeatInitial: 200, repeatHeld: 20 };
const BUTTON_OPTS_SLOW = { repeatInitial: 200, repeatHeld: 200 };

// Utility
const randInt = (min, max) => Math.floor(Math.random() * (max - min)) + min;
const sleep = (ms) => new Promise(resolve => setTimeout(resolve, ms));
const until = async (f) => { while (!f()) await sleep(1/30); }
function* sequence(a, b) {
    for (let cur = a; cur < b; cur++)
        yield cur;
}

const IS_COLOR = (type) => TILE_START <= type && type <= NUM_TYPES;
const ON_FIELD = (row, col, height, width) => row >= 0 && row < height && col >= 0 && col < width;

PIXI.Rectangle.prototype.asArray = function () { return [this.x, this.y, this.width, this.height]; }

// Tile class
class Tile {
    static rand() { return randInt(TILE_START, NUM_TYPES+1); }

    constructor(type = null, container = null) {
        if (type === null) {
            this.type = TILE_EMPTY;
        } else {
            this.type = type;
        }
        this.age = 0;
        this.frame = 0;
        this.started = false;
        this.sprite = null;

        if (container)
          this.set_sprite(container);
    }

    remove_sprite() {
        this.sprite?.parent?.removeChild(this.sprite);
    }

    set_sprite(container) {
        if (this.sprite) {
            this.remove_sprite();
        } else {
            this.sprite = new PIXI.Sprite();
            this.sprite.width = TILE_SIZE;
            this.sprite.height = TILE_SIZE;
        }
        container.addChild(this.sprite);
    }

    update_field_sprite(game, row, col, connected) {
        if (!this.sprite) return;
        if (this.type === TILE_EMPTY) {
            this.remove_sprite();
            this.sprite.visible = false;
            return;
        }
        this.sprite.visible = true;
        let key;
        if (this.type === TILE_CLEARED) {
            if (this.frame == 6) {
                this.type = TILE_EMPTY;
                this.remove_sprite();
                return;
            }
            key = `coin${this.frame + 1}`;
        } else if (IS_COLOR(this.type)) {
            key = game.get_texture_key(this.type, connected);
        }
        this.sprite.texture = game.textures[key];
        this.sprite.x = col * TILE_SIZE;
        this.sprite.y = (game.field.height - 1 - row) * TILE_SIZE;
    }

    update_block_sprite(game, x, y) {
        if (!this.sprite) return;
        if (IS_COLOR(this.type)) {
            this.sprite.visible = true;
            const key = game.get_texture_key(this.type, false);
            this.sprite.texture = game.textures[key];
            this.sprite.x = x;
            this.sprite.y = y;
        } else {
            this.sprite.visible = false;
        }
    }

    tick(ms) {
        if (this.started) {
            this.age += ms;
            this.frame = Math.min(6, Math.floor(this.age / 66)); // 6 frames, ~66ms each for 400ms total
        }
    }

    start() {
        this.started = true;
        this.age = 0;
        this.frame = 0;
    }
}

// Block class
class Block {
    constructor(game, y = null, x = null, container = null) {
        this.orient = NORTH;
        this.tiles = [];
        for (let i = 0; i < BLOCK_SIZE; i++) {
            this.tiles.push(new Tile(Tile.rand(), container));
        }
        this.y = y !== null ? y : 0;
        this.x = x !== null ? x : 0;
        this.dy = 1.0;
        this.dx = 0.0;
        this.timer = 0;
        this.start_angle = 0;
        this.stop_angle = 0;
        this.running = false;
    }

    mimic(b) {
        this.orient = b.orient;
        this.x = b.x;
        this.y = b.y;
    }

    get_y(n) {
        return this.y + n * this.dy;
    }

    get_x(n) {
        return this.x + n * this.dx;
    }

    get_row(n) {
        if (this.orient === NORTH) {
            return Math.floor(this.y + n);
        }
        if (this.orient === SOUTH) {
            return Math.floor(this.y - n);
        }
        return Math.floor(this.y);
    }

    get_col(n) {
        if (this.orient === EAST) {
            return Math.floor(this.x + n);
        }
        if (this.orient === WEST) {
            return Math.floor(this.x - n);
        }
        return Math.floor(this.x);
    }

    move_up(n = 1.0) { this.y += n; }
    move_down(n = 1.0) { this.y -= n; }
    move_left(n = 1.0) { this.x -= n; }
    move_right(n = 1.0) { this.x += n; }

    tick(ms) {
        if (!this.running) return;

        this.timer += ms;

        if (this.timer >= 150) { // ROTATION_DURATION
            this.stop();
            this.set_offsets();
            return;
        }

        const t = this.timer / 150.0;
        const angle = (1 - t) * this.start_angle + t * this.stop_angle;
        this.dy = Math.sin(angle);
        this.dx = Math.cos(angle);
    }

    rotate_right(anim = false) {
        const old_orient = this.orient;
        this.orient = (this.orient + 1) % 4;
        if (anim) {
            this.timer = 0;
            this.start_angle = this.get_angle(old_orient);
            this.stop_angle = this.get_angle(this.orient);
            if (this.stop_angle > this.start_angle) {
                this.start_angle += 2 * Math.PI;
            }
            this.start();
        } else {
            this.set_offsets();
        }
    }

    rotate_left(anim = false) {
        const old_orient = this.orient;
        this.orient = (this.orient + 4 - 1) % 4;
        if (anim) {
            this.timer = 0;
            this.start_angle = this.get_angle(old_orient);
            this.stop_angle = this.get_angle(this.orient);
            if (this.stop_angle < this.start_angle) {
                this.stop_angle += 2 * Math.PI;
            }
            this.start();
        } else {
            this.set_offsets();
        }
    }

    get_angle(orient) {
        switch (orient) {
            case EAST: return 0;
            case NORTH: return Math.PI / 2;
            case WEST: return Math.PI;
            case SOUTH: return 3 * Math.PI / 2;
        }
        return 0;
    }

    set_offsets() {
        switch (this.orient) {
            case NORTH:
                this.dx = 0.0;
                this.dy = 1.0;
                break;
            case EAST:
                this.dx = 1.0;
                this.dy = 0.0;
                break;
            case SOUTH:
                this.dx = 0.0;
                this.dy = -1.0;
                break;
            case WEST:
                this.dx = -1.0;
                this.dy = 0.0;
                break;
        }
    }

    start() {
        this.running = true;
    }

    stop() {
        this.running = false;
    }

    get_tile(i) {
        return this.tiles[i];
    }

    get_orient() {
        return this.orient;
    }

    update_sprite(game, startY) {
        sequence(0, BLOCK_SIZE).forEach(i => {
            this.tiles[i].update_block_sprite(game, this.get_x(i)*TILE_SIZE, startY - this.get_y(i)*TILE_SIZE);
        });
    }
}

// Field class
class Field {
    constructor(game, width = FIELD_WIDTH, height = FIELD_HEIGHT) {
        this.game = game;
        this.width = width;
        this.height = height;
        this.data = [];
        this.connected = [];
        for (let i = 0; i < height; i++) {
            this.data[i] = [];
            this.connected[i] = [];
            for (let j = 0; j < width; j++) {
                this.data[i][j] = new Tile(TILE_EMPTY, this.game.field_container);
                this.connected[i][j] = false;
            }
        }
        this.current = null;
        this.next = new Block(this.game, 0, 0, this.game.next_container);
        this.state = BLOCK_RELEASE;
        this.score = 0;
        this.combo = 0;
        this.best_combo = 0;
        this.speed = 0;
        this.hazard_num = 0;
        this.hazard_timer = 0;
        this.timer = 0;
        this.field_changed = true;
    }

    game_over() {
        return this.state === GAME_OVER;
    }

    get_score() { return this.score; }
    get_best_combo() { return this.best_combo; }
    get_speed() { return this.speed; }
    get_hazard_percent() { return Math.min(1, this.hazard_timer / (HAZARD_INTERVAL * 1000)); }
    get_next() { return this.next; }
    get_block() { return this.current; }
    get_tile(row, col) { return this.data[row][col]; }
    is_connected(row, col) { return this.connected[row][col]; }

    tick(ms) {
        // Tick all tiles
        for (let row = 0; row < this.height; row++) {
            for (let col = 0; col < this.width; col++) {
                this.data[row][col].tick(ms);
            }
        }
        if (this.current) {
          this.current.tick(ms);
        }

        this.timer += ms;

        if (this.state !== WIPE_CLEARED) {
            this.hazard_timer += ms;
        }

        switch (this.state) {
            case BLOCK_RELEASE:
                this.block_release();
                this.timer = 0;
                break;
            case BLOCK_FALLING:
                if (this.timer >= (325 - this.speed * 13)) {
                    this.block_move_down();
                    this.timer = 0;
                }
                break;
            case FORM_CONNECTIONS:
                if (this.timer >= 400) {
                    this.form_connections();
                    this.timer = 0;
                }
                break;
            case CLEAR_SETS:
                if (this.timer >= 400) {
                    this.clear_sets();
                    this.timer = 0;
                }
                break;
            case WIPE_CLEARED:
                // Simplified, no sprite duration
                if (this.timer >= 450) {
                    this.wipe_cleared();
                    this.timer = 0;
                }
                break;
            case HAZARD_CHECK:
                this.hazard_check();
                break;
            case GAME_OVER:
                break;
        }
    }

    check_collision(block) {
        let num_collide = 0;
        for (let i = 0; i < BLOCK_SIZE; i++) {
            const row = block.get_row(i);
            const col = block.get_col(i);
            if (row >= this.height && ON_FIELD(0, col, this.height, this.width)) continue;
            if (!ON_FIELD(row, col, this.height, this.width) || this.data[row][col].type !== TILE_EMPTY) {
                num_collide++;
            }
        }
        return num_collide;
    }

    block_release() {
        this.current = this.next;
        this.current.y = this.height - 0.5;
        this.current.x = this.width / 2;

        if (this.check_collision(this.current)) {
            this.state = GAME_OVER;
            return;
        }
        for (let i = 0; i < BLOCK_SIZE; i++) {
            this.current.tiles[i].set_sprite(this.game.field_container);
        }
        this.next = new Block(this.game, 0, 0, this.game.next_container);
        this.state = BLOCK_FALLING;
    }

    block_move_down() {
        if (this.state !== BLOCK_FALLING) return false;
        const b = new Block();
        b.mimic(this.current);
        b.move_down(0.5);
        if (!this.check_collision(b)) {
            this.current.mimic(b);
            return true;
        }
        this.block_lock();
        return false;
    }

    block_move_left() {
        if (this.state !== BLOCK_FALLING) return false;
        const b = new Block();
        b.mimic(this.current);
        b.move_left();
        if (!this.check_collision(b)) {
            this.current.mimic(b);
            return true;
        }
        return false;
    }

    block_move_right() {
        if (this.state !== BLOCK_FALLING) return false;
        const b = new Block();
        b.mimic(this.current);
        b.move_right();
        if (!this.check_collision(b)) {
            this.current.mimic(b);
            return true;
        }
        return false;
    }

    block_rotate_left() {
        if (this.state !== BLOCK_FALLING) return false;
        let n;
        const b = new Block();
        b.mimic(this.current);
        b.rotate_left(false);
        if ((n = this.check_collision(b)) === 0) {
            this.current.rotate_left(true);
            return true;
        }
        switch (this.current.get_orient()) {
            case NORTH:
                b.move_right(n);
                break;
            case SOUTH:
                b.move_left(n);
                break;
            default:
                return false;
        }
        if (!this.check_collision(b)) {
            b.rotate_right(false);
            this.current.mimic(b);
            this.current.rotate_left(true);
            return true;
        }
        return false;
    }

    block_rotate_right() {
        if (this.state !== BLOCK_FALLING) return false;
        let n;
        const b = new Block();
        b.mimic(this.current);
        b.rotate_right(false);
        if ((n = this.check_collision(b)) === 0) {
            this.current.rotate_right(true);
            return true;
        }
        switch (this.current.get_orient()) {
            case NORTH:
                b.move_left(n);
                break;
            case SOUTH:
                b.move_right(n);
                break;
            default:
                return false;
        }
        if (!this.check_collision(b)) {
            b.rotate_left(false);
            this.current.mimic(b);
            this.current.rotate_right(true);
            return true;
        }
        return false;
    }

    block_lock() {
        if (this.state !== BLOCK_FALLING) return false;
        for (let i = 0; i < BLOCK_SIZE; i++) {
            let n = i;
            if (this.current.get_orient() === SOUTH) n = BLOCK_SIZE - 1 - i;
            let row = this.current.get_row(n);
            let col = this.current.get_col(n);
            while (row > 0 && (row >= this.height || this.data[row - 1][col].type === TILE_EMPTY)) row--;
            if (row >= this.height || this.data[row][col].type !== TILE_EMPTY) {
                this.state = GAME_OVER;
                return false;
            }
            this.data[row][col] = this.current.get_tile(n);
        }
        this.current = null;
        this.state = FORM_CONNECTIONS;
        return true;
    }

    form_connections() {
        let new_connection = false;
        for (let row = 0; row < this.height; row++) {
            for (let col = 0; col < this.width; col++) {
                const tile = this.get_tile(row, col);
                if (IS_COLOR(tile.type) &&
                    ((col + 1 < this.width && tile.type === this.get_tile(row, col + 1).type) ||
                     (row + 1 < this.height && tile.type === this.get_tile(row + 1, col).type) ||
                     (col - 1 >= 0 && tile.type === this.get_tile(row, col - 1).type) ||
                     (row - 1 >= 0 && tile.type === this.get_tile(row - 1, col).type))) {
                    if (!this.connected[row][col]) {
                        new_connection = true;
                        this.connected[row][col] = true;
                    }
                } else {
                    this.connected[row][col] = false;
                }
            }
        }
        if (new_connection) {
            this.game.play_sound('lock');
        }
        this.state = CLEAR_SETS;
    }

    calc_score(num_cleared) {
        const tbonus = 1 + (num_cleared - CLEAR_MIN) / 2.0;
        const cbonus = Math.pow(3, this.combo);
        const bonus = tbonus * cbonus;
        return [Math.floor(bonus * POINTS_PER_CLEAR), bonus];
    }

    add_score(num_cleared) {
        if (num_cleared === 0) {
            if (this.combo > this.best_combo) this.best_combo = this.combo;
            this.combo = 0;
            this.state = HAZARD_CHECK;
            return 0;
        } else {
            const [gain, bonus] = this.calc_score(num_cleared);
            this.combo++;
            this.score += gain;
            this.hazard_decrease(Math.floor(bonus * HAZARD_DROP_PER_CLEAR));
            this.game.play_sound(`clear${Math.min(this.combo, 5)}`);
            this.state = WIPE_CLEARED;
            return gain;
        }
    }

    clear_sets() {
        let total_cleared = 0;
        for (let row = 0; row < this.height; row++) {
            for (let col = 0; col < this.width; col++) {
                if (!IS_COLOR(this.data[row][col].type)) continue;
                if (this.check_set_from(row, col, this.data[row][col].type) >= CLEAR_MIN) {
                    let num_cleared = this.clear_set_from(row, col, this.data[row][col].type);
                    const [gain, bonus] = this.calc_score(num_cleared);
                    this.game.add_floating_text(gain, row, col);
                    total_cleared += num_cleared;
                }
            }
        }
        this.add_score(total_cleared);
    }

    check_set_from(row, col, type) {
        if (!ON_FIELD(row, col, this.height, this.width) || this.data[row][col].type !== type) return 0;
        const tmp = this.data[row][col];
        this.data[row][col] = new Tile(TILE_CLEARED);
        let num = 1;
        num += this.check_set_from(row - 1, col, type);
        num += this.check_set_from(row + 1, col, type);
        num += this.check_set_from(row, col - 1, type);
        num += this.check_set_from(row, col + 1, type);
        this.data[row][col] = tmp;
        return num;
    }

    clear_set_from(row, col, type) {
        if (!ON_FIELD(row, col, this.height, this.width) || this.data[row][col].type !== type) return 0;
        this.connected[row][col] = false;
        this.data[row][col].remove_sprite();
        this.data[row][col] = new Tile(TILE_CLEARED, this.game.field_container);
        this.data[row][col].start();
        let num = 1;
        num += this.clear_set_from(row - 1, col, type);
        num += this.clear_set_from(row + 1, col, type);
        num += this.clear_set_from(row, col - 1, type);
        num += this.clear_set_from(row, col + 1, type);
        return num;
    }

    wipe_cleared() {
        for (let col = 0; col < this.width; col++) {
            let write_row = 0;
            for (let read_row = 0; read_row < this.height; read_row++) {
                if (IS_COLOR(this.data[read_row][col].type)) {
                    this.connected[write_row][col] = this.connected[read_row][col];
                    this.data[write_row][col] = this.data[read_row][col];
                    write_row++;
                }
            }
            for (let r = write_row; r < this.height; r++) {
                this.data[r][col] = new Tile(TILE_EMPTY);
                this.connected[r][col] = false;
            }
        }
        this.state = FORM_CONNECTIONS;
    }

    hazard_check() {
        if (this.hazard_timer >= 1000 * HAZARD_INTERVAL) {
            this.hazard_timer = 0;
            this.hazard_num++;
            if ((this.hazard_num % 2) === 1) {
                this.game.play_sound('speedup');
                this.speed++;
            } else {
                this.game.play_sound('tiledump');
                this.hazard_tiledump();
            }
        }
        this.state = BLOCK_RELEASE;
    }

    hazard_tiledump() {
        for (let col = 0; col < this.width; col++) {
            if (IS_COLOR(this.data[this.height - 1][col].type)) {
                this.state = GAME_OVER;
                return;
            }
        }
        for (let row = this.height - 1; row > 0; row--) {
            for (let col = 0; col < this.width; col++) {
                this.data[row][col] = this.data[row - 1][col];
            }
        }
        for (let col = 0; col < this.width; col++) {
            let attempts = 50;
            do {
                this.data[0][col] = new Tile(Tile.rand(), this.game.field_container);
                attempts--;
            } while (attempts > 0 &&
                      ((col >= 2 && this.data[0][col - 2].type === this.data[0][col - 1].type && this.data[0][col - 1].type === this.data[0][col].type) ||
                       (this.data[0][col].type === this.data[1][col].type)));
        }
        this.form_connections();
    }

    hazard_decrease(percent) {
        this.hazard_timer -= percent * 10 * HAZARD_INTERVAL;
        if (this.hazard_timer < 0) this.hazard_timer = 0;
    }

    has_field_changed() { return this.field_changed; }
    set_field_changed(val) { this.field_changed = val; }
}

// Game class
class Game {
    constructor() {
        this.started = false;
        this.music_on = true;
        this.sfx_on = true;

        this.textures = {};
        this.textures_loaded = false;
        this.sounds = {};
        this.floating_texts = [];

        this.scoreText = null;
        this.speedText = null;
        this.comboText = null;
        this.progressBar = null;
        this.fieldBounds = null;
        this.musicBtn = null;
        this.sfxBtn = null;

        this.gameOverShown = false;
        this.gameOverOverlay = null;

        this.setup_start_button();
    }

    async init() {
        await this.load_textures();
        await this.load_sounds();
        await this.init_app();
    }

    get desiredWidth() {
        return TILE_SIZE*FIELD_WIDTH + 280;
    }

    get desiredHeight() {
        return TILE_SIZE*FIELD_HEIGHT + 100;
    }

    async init_app() {
        this.app = new PIXI.Application();
        await this.app.init({
            resizeTo: window,
            backgroundAlpha: 0,
        });
        document.body.appendChild(this.app.canvas);
        this.app.canvas.style.display = 'none';

        {
            this.field_container = new PIXI.Container({x: 10, y: 10});

            const container = this.field_container;
            const bg = new PIXI.Graphics().rect(0, 0, TILE_SIZE*FIELD_WIDTH, TILE_SIZE*FIELD_HEIGHT).fill(0x999999);
            container.addChild(bg);

            const mask = bg.clone();
            container.mask = mask;
            container.addChild(mask);

            // Add shadow
            let shadowBounds = this.field_container.getBounds();
            this.fieldBounds =  this.field_container.getBounds();

            shadowBounds.pad(3);
            const shadow = new PIXI.Graphics().rect(shadowBounds.left, shadowBounds.top, shadowBounds.width, shadowBounds.height).fill({color: 0x000000, alpha: 0.3});
            this.app.stage.addChild(shadow);

            // Add container
            this.app.stage.addChild(container);
        }

        // Next block viewport
        {
            this.next_container = new PIXI.Container();

            const bg = new PIXI.Graphics().rect(0, 0, TILE_SIZE, TILE_SIZE*2).fill(0x999999);
            this.next_container.addChild(bg);
            const mask = bg.clone();
            this.next_container.mask = mask;
            this.next_container.addChild(mask);

            const fieldBounds = this.field_container.getBounds();
            this.next_container.x = fieldBounds.right + 25;
            this.next_container.y = fieldBounds.top;
            // this.next_container.y = fieldBounds.top * this.app.stage.scale.y;

            // Add shadow
            let shadowBounds = this.next_container.getBounds();
            shadowBounds.pad(3);
            const shadow = new PIXI.Graphics().rect(shadowBounds.left, shadowBounds.top, shadowBounds.width, shadowBounds.height).fill({color: 0x000000, alpha: 0.3});
            this.app.stage.addChild(shadow);

            // Add container
            this.app.stage.addChild(this.next_container);
        }

        this.setup_ui();

        const tmp = new PIXI.Graphics().rect(0, 0, this.desiredWidth, this.desiredHeight)
        .fill({color: 0x000011, alpha: 0.3});
        this.app.stage.addChild(tmp);

        const scaleW = this.app.screen.width / this.desiredWidth;
        const scaleH = this.app.screen.height / this.desiredHeight;
        const scale = Math.min(scaleW, scaleH);
        // const scale = Math.min(1, Math.min(scaleW, scaleH));
        let container = this.app.stage;
        container.scale.set(scale);
        container.x = (this.app.screen.width / 2 - this.desiredWidth/2 * scale);
        container.y = this.app.screen.height / 2 - this.desiredHeight/2 * scale;
    }

    setup_ui() {
        const fieldBounds = this.field_container.getBounds();
        const nextBounds = this.next_container.getBounds();

        let curX = nextBounds.left;
        let curY = nextBounds.bottom + 100;
        const spacing = 60;

        // Score text
        this.scoreText = new PIXI.BitmapText({
            text: 'Score: 0',
            style: FONT_STYLE,
        });
        this.scoreText.x = curX;
        this.scoreText.y = curY;
        this.app.stage.addChild(this.scoreText);
        curY += spacing;

        // Speed text
        this.speedText = new PIXI.BitmapText({
            text: 'Speed: 0',
            style: FONT_STYLE,
        });
        this.speedText.x = curX;
        this.speedText.y = curY;
        this.app.stage.addChild(this.speedText);
        curY += spacing;

        // Combo text
        this.comboText = new PIXI.BitmapText({
            text: 'Best Combo: 0',
            style: FONT_STYLE,
        });
        this.comboText.x = curX;
        this.comboText.y = curY;
        this.app.stage.addChild(this.comboText);
        curY += spacing;

        curY = fieldBounds.bottom - spacing*2;
        // Music button
        this.musicBtn = this.create_bitmap_button('Music: ON', [curX, curY, 220, 50], () => {
            this.music_on = !this.music_on;
            this.musicBtn.text.text = `Music: ${this.music_on ? 'ON' : 'OFF'}`;
            if (this.sounds.bgm) this.sounds.bgm.volume = this.music_on ? 1 : 0;
        }, {repeatInitial: 1000, repeatHeld: 1000});
        curY += spacing;

        // SFX button
        this.sfxBtn = this.create_bitmap_button('SFX: ON', [curX, curY, 220, 50], () => {
            this.sfx_on = !this.sfx_on;
            this.sfxBtn.text.text = `SFX: ${this.sfx_on ? 'ON' : 'OFF'}`;
        }, {repeatInitial: 1000, repeatHeld: 1000});
        curY += spacing;

        // Progress bar background
        let bounds = new PIXI.Rectangle(fieldBounds.left, fieldBounds.bottom + 10, fieldBounds.width, 24);
        bounds.pad(3);
        const progressBg = new PIXI.Graphics().rect(bounds.x, bounds.y, bounds.width, bounds.height).fill(0x333333);
        this.app.stage.addChild(progressBg);

        // Progress bar
        this.progressBar = new PIXI.Graphics();
        this.progressBar.x = fieldBounds.left;
        this.progressBar.y = fieldBounds.bottom + 10;
        this.app.stage.addChild(this.progressBar);

        if (this.is_mobile()||true) {
            // Mobile buttons at bottom
            const btnY = this.desiredHeight - 50;
            const btnSpacing = 68;
            let btnX = 10;
            let [w, h] = [50, 50];
            // let btnX = this.desiredWidth / 2 - btnSpacing * 2;

            this.create_arial_button('↺', [btnX, btnY, w, h], () => this.field.block_rotate_left(), BUTTON_OPTS_SLOW);
            btnX += btnSpacing;
            this.create_arial_button('←', [btnX, btnY, w, h], () => this.field.block_move_left());
            btnX += btnSpacing;
            this.create_arial_button('↓', [btnX, btnY, w, h], () => this.field.block_move_down());
            btnX += btnSpacing;
            this.create_arial_button('→', [btnX, btnY, w, h], () => this.field.block_move_right());
            btnX += btnSpacing;
            this.create_arial_button('↻', [btnX, btnY, w, h], () => this.field.block_rotate_right(), BUTTON_OPTS_SLOW);
        }
    }

    setup_start_button() {
        const startBtn = document.getElementById('start-button');
        startBtn.addEventListener('click', () => {
            this.app.canvas.style.display = 'block';
            document.getElementById('start-screen').style.display = 'none';
            this.start_game();
        });
    }

    create_button_common([x, y, w, h], callback, text, opts = BUTTON_OPTS) {
        let btn = new PIXI.Container();

        let bgRect = new PIXI.Rectangle(0, 0, w, h);
        const bgOut = new PIXI.Graphics().rect(...bgRect.asArray()).fill(0x80621d);
        btn.addChild(bgOut);
        bgRect.pad(-3);
        const bgIn = new PIXI.Graphics().rect(...bgRect.asArray()).fill(0xa88532);
        btn.addChild(bgIn);
        btn.addChild(text);
        text.x = w / 2;
        text.y = h / 2;
        text.pivot.x = text.width / 2;
        text.pivot.y = text.height / 2;
        this.app.stage.addChild(btn);

        btn.text = text;
        btn.x = x;
        btn.y = y;
        btn.interactive = true;
        btn.repeat = null;
        btn.on('pointerdown', () => {
            callback();
            btn.repeat = setTimeout(() => {
                callback();
                btn.repeat = setInterval(() => {
                    callback();
                }, opts.repeatHeld);
            }, opts.repeatInitial);
        });
        btn.on('pointerup', () => {
            clearTimeout(btn.repeat);
        });
        btn.on('pointerupoutside', () => {
            clearTimeout(btn.repeat);
        });
        btn.on('pointerover', () => {
            bgIn.clear();
            bgIn.rect(...bgRect.asArray()).fill(0x80621d);
            // btn.style = Object.assign({}, FONT_STYLE, { fill: 0x773300 });
        });
        btn.on('pointerout', () => {
            bgIn.clear();
            bgIn.rect(...bgRect.asArray()).fill(0xa88532);
            // btn.style = FONT_STYLE;
        });

        return btn;
    }

    create_bitmap_button(text, dims, callback, opts = BUTTON_OPTS) { 
        let btn = this.create_button_common(dims, callback, new PIXI.BitmapText({
            text: text,
            style: FONT_STYLE,
        }), opts);
        btn.text.pivot.y += 15; // bitmap text is a bit off vertically
        return btn;
    }
    create_arial_button(text, dims, callback, opts = BUTTON_OPTS) {
        let btn = this.create_button_common(dims, callback, new PIXI.Text({
            text: text,
            style: { fontFamily: 'Arial', fontSize: 36 },
        }), opts);
        btn.text.pivot.y += 5;
        return btn;
    }

    is_mobile() {
        return window.innerWidth < 1000 || 'ontouchstart' in window;
    }

    start_game() {
        this.started = true;
        this.field = new Field(this);
        this.last_time = performance.now();
        this.key_delay = new Array(NUM_KEYS).fill(0);
        this.key_handled = new Array(NUM_KEYS).fill(false);
        this.paused = false;

        this.setup_input();
        this.app.ticker.add(this.update.bind(this));
        this.start_bgm();
        this.show_game_over();
    }

    async load_sounds() {
        this.sounds.bgm = new Audio('/blingbling/assets/sounds/background.mp3');
        this.sounds.bgm.loop = true;
        this.sounds.lock = new Audio('/blingbling/assets/sounds/lock.wav');
        this.sounds.speedup = new Audio('/blingbling/assets/sounds/speedup.wav');
        this.sounds.tiledump = new Audio('/blingbling/assets/sounds/tiledump.wav');
        for (let i = 1; i <= 5; i++) {
            this.sounds[`clear${i}`] = new Audio(`/blingbling/assets/sounds/clear${i}.wav`);
        }
    }

    async start_bgm() {
        await until(() => this.sounds.bgm);
        this.sounds.bgm.play();
    }

    play_sound(name) {
        if (this.sfx_on && this.sounds[name]) {
            this.sounds[name].currentTime = 0;
            this.sounds[name].play();
        }
    }

    add_floating_text(gain, row, col) {
        const text = new PIXI.BitmapText({
            text: `+${gain}`,
            style: FONT_STYLE_LARGE,
            anchor: 0.5,
        });
        const fieldBounds = this.fieldBounds;
        const scale = this.app.stage.scale;
        console.log('float', scale, fieldBounds)
        text.x = fieldBounds.left + col * TILE_SIZE;
        text.y = fieldBounds.bottom - row * TILE_SIZE;
        this.app.stage.addChild(text);
        this.floating_texts.push({ text, y: text.y, time: 0 });
    }

    update_floating_texts(ms) {
        for (let i = this.floating_texts.length - 1; i >= 0; i--) {
            const ft = this.floating_texts[i];
            ft.time += ms;
            ft.text.y = ft.y - ft.time / 10; // move up
            ft.text.alpha = 1 - ft.time / 1000; // fade out
            if (ft.time > 1000) {
                this.app.stage.removeChild(ft.text);
                this.floating_texts.splice(i, 1);
            }
        }
    }

    async load_textures() {
        // Font
        await PIXI.Assets.load('https://pixijs.com/assets/bitmap-font/desyrel.xml');

        // Load normal and connected
        for (let i = 0; i < NUM_TYPES; i++) {
            const name = TILE_NAMES[i];
            this.textures[`${name}`] = await PIXI.Assets.load(`assets/tiles/${name}.png`);
            this.textures[`c${name}`] = await PIXI.Assets.load(`assets/tiles/c${name}.png`);
        }
        // Load clear animation
        for (let i = 1; i <= 6; i++) {
            this.textures[`coin${i}`] = await PIXI.Assets.load(`assets/tiles/coin${i}.png`);
        }
        this.textures_loaded = true;
    }

    setup_input() {
        const key_map = {
            'KeyS': KEY_DOWN,
            'KeyA': KEY_LEFT,
            'KeyD': KEY_RIGHT,
            'KeyQ': KEY_ROTATE_LEFT,
            'KeyE': KEY_ROTATE_RIGHT,
            'Space': KEY_DROP
        };
        window.addEventListener('keydown', (e) => {
            const key = key_map[e.code];
            if (key !== undefined) {
                e.preventDefault();
                this.key_delay[key] = performance.now();
                this.key_handled[key] = false;
            }
        });

        window.addEventListener('keyup', (e) => {
            const key = key_map[e.code];
            if (key !== undefined) {
                this.key_delay[key] = 0;
            }
        });
    }

    update() {
        if (!this.started) return;

        const now = performance.now();
        const ms = now - this.last_time;
        this.last_time = now;

        if (this.field.game_over()) {
            if (!this.gameOverShown) {
                this.show_game_over();
                this.gameOverShown = true;
            }
            return;
        }

        if (this.paused) return;

        const curtime = now;

        // Handle keys
        const handle_key = (i, delay, interval, func) => {
            if (this.key_delay[i] && curtime >= this.key_delay[i]) {
                func();
                this.key_delay[i] = !this.key_handled[i] ? (curtime + delay) : (curtime + interval);
                this.key_handled[i] = true;
            }
        };

        handle_key(KEY_DOWN, 0, 0, () => this.field.block_move_down());
        handle_key(KEY_LEFT, 200, 0, () => this.field.block_move_left());
        handle_key(KEY_RIGHT, 200, 0, () => this.field.block_move_right());
        handle_key(KEY_DROP, 250, 200, () => this.field.block_lock());
        if (this.key_delay[KEY_ROTATE_RIGHT] && !this.key_handled[KEY_ROTATE_RIGHT]) {
            this.field.block_rotate_right();
            this.key_handled[KEY_ROTATE_RIGHT] = true;
        }
        if (this.key_delay[KEY_ROTATE_LEFT] && !this.key_handled[KEY_ROTATE_LEFT]) {
            this.field.block_rotate_left();
            this.key_handled[KEY_ROTATE_LEFT] = true;
        }

        this.field.tick(ms);

        // Update UI
        this.scoreText.text = `Score: ${this.field.get_score()}`;
        this.speedText.text = `Speed: ${this.field.get_speed()}`;
        this.comboText.text = `Best Combo: ${this.field.get_best_combo()}`;

        // Update progress bar
        this.progressBar.clear();
        const percent = this.field.get_hazard_percent();
        this.progressBar.rect(0, 0, this.field_container.width * percent, 24).fill(0xff0000);

        this.update_floating_texts(ms);

        // Render
        this.render();
    }

    get_texture_key(type, connected) {
        const name = TILE_NAMES[type - 1];
        return connected ? `c${name}` : name;
    }

    show_game_over() {
        // Create overlay
        this.gameOverOverlay = new PIXI.Container();

        // Semi-transparent background
        // const w = this.desiredWidth;
        // const h = this.desiredHeight;
        const w = this.app.screen.width;
        const h = this.app.screen.height;
        const bg = new PIXI.Graphics().rect(0, 0, w, h).fill({color: 0x000000, alpha: 0.7});
        this.gameOverOverlay.addChild(bg);

        this.gameOverOverlay.x = this.app.stage.width/2;
        this.gameOverOverlay.y = this.app.stage.height/2 + 10;
        this.gameOverOverlay.pivot.x = this.gameOverOverlay.width / 2;
        this.gameOverOverlay.pivot.y = this.gameOverOverlay.height / 2;
        console.log('GO overlay', this.gameOverOverlay.width, this.gameOverOverlay.height, w, h);
        console.log('stags', this.app.stage.width, this.app.stage.height);

        // Game Over text
        const gameOverText = new PIXI.BitmapText({
            text: 'GAME OVER',
            style: FONT_STYLE_LARGE,
        });
        gameOverText.anchor.set(0.5);
        gameOverText.y = -150;
        this.gameOverOverlay.addChild(gameOverText);

        // Final Score
        const scoreText = new PIXI.BitmapText({
            text: `Final Score: ${this.field.get_score()}`,
            style: FONT_STYLE,
        });
        scoreText.anchor.set(0.5);
        scoreText.y = -50;
        this.gameOverOverlay.addChild(scoreText);

        // Best Combo
        const comboText = new PIXI.BitmapText({
            text: `Best Combo: ${this.field.get_best_combo()}`,
            style: FONT_STYLE,
        });
        comboText.anchor.set(0.5);
        comboText.y = 0;
        this.gameOverOverlay.addChild(comboText);

        // Restart button
        const restartBtn = new PIXI.BitmapText({
            text: 'RESTART',
            style: FONT_STYLE,
        });
        restartBtn.anchor.set(0.5);
        restartBtn.y = 100;
        restartBtn.interactive = true;
        restartBtn.on('pointerdown', () => {
            window.location.reload();
        });
        restartBtn.on('pointerover', () => {
            restartBtn.style = Object.assign({}, FONT_STYLE, { fill: 0x773300 });
        });
        restartBtn.on('pointerout', () => {
            restartBtn.style = FONT_STYLE;
        });
        this.gameOverOverlay.addChild(restartBtn);

        this.app.stage.addChild(this.gameOverOverlay);
    }

    render() {
        if (!this.textures_loaded) return;

        // Update field sprites
        for (let row = 0; row < this.field.height; row++) {
            for (let col = 0; col < this.field.width; col++) {
                this.field.get_tile(row, col).update_field_sprite(this, row, col, this.field.is_connected(row, col));
            }
        }

        // Update current block sprites
        if (this.field.current) {
            this.field.current.update_sprite(this, (this.field.height - 1) * TILE_SIZE);
        }

        // Update next block sprites
        const next = this.field.get_next();
        if (next) {
            next.update_sprite(this, TILE_SIZE);
        }
    }
}

// Start game
const game = new Game();
game.init();