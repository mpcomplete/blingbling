// Constants
const TILE_EMPTY = -1;
const TILE_CLEARED = 0;
const TILE_START = 1;
const NUM_TYPES = 5; // colors
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

// Utility
function randInt(min, max) {
    return Math.floor(Math.random() * (max - min + 1)) + min;
}

function IS_COLOR(type) {
    return TILE_START <= type && type <= NUM_TYPES;
}

function ON_FIELD(row, col, height, width) {
    return row >= 0 && row < height && col >= 0 && col < width;
}

// Tile class
class Tile {
    constructor(type = null) {
        if (type === null) {
            this.type = randInt(TILE_START, NUM_TYPES);
        } else {
            this.type = type;
        }
        this.age = 0;
        this.frame = 0;
    }

    tick(ms) {
        this.age += ms;
        // Animation logic if needed
    }

    start() {}
}

// Block class
class Block {
    constructor(y = null, x = null) {
        this.orient = NORTH;
        this.tiles = [];
        for (let i = 0; i < BLOCK_SIZE; i++) {
            this.tiles.push(new Tile());
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

    start_tiles() {
        // In JS, no global ticker, just set running
    }

    stop_tiles() {
        //
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
}

// Field class
class Field {
    constructor(width = FIELD_WIDTH, height = FIELD_HEIGHT) {
        this.width = width;
        this.height = height;
        this.data = [];
        this.connected = [];
        for (let i = 0; i < height; i++) {
            this.data[i] = [];
            this.connected[i] = [];
            for (let j = 0; j < width; j++) {
                this.data[i][j] = new Tile(TILE_EMPTY);
                this.connected[i][j] = false;
            }
        }
        this.current = null;
        this.next = new Block(height - 0.5, width / 2);
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
    get_hazard_percent() { return this.hazard_timer / (HAZARD_INTERVAL * 1000); }
    get_next() { return this.next; }
    get_block() { return this.current; }
    get_tile(row, col) { return this.data[row][col]; }
    is_connected(row, col) { return this.connected[row][col]; }

    tick(ms) {
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
                if (this.timer >= 400) {
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
        if (this.check_collision(this.next)) {
            this.state = GAME_OVER;
            return;
        }
        this.current = this.next;
        this.next = new Block(this.height - 0.5, this.width / 2);
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
        this.current.stop_tiles();
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
            // Play sound
        }
        this.state = CLEAR_SETS;
    }

    add_score(num_cleared) {
        if (num_cleared === 0) {
            if (this.combo > this.best_combo) this.best_combo = this.combo;
            this.combo = 0;
            this.state = HAZARD_CHECK;
            return 0;
        } else {
            this.combo++;
            const tbonus = 1 + (num_cleared - CLEAR_MIN) / 2.0;
            const cbonus = Math.pow(3, this.combo - 1);
            const bonus = tbonus * cbonus;
            const gain = Math.floor(bonus * POINTS_PER_CLEAR);
            this.score += gain;
            this.hazard_decrease(Math.floor(bonus * HAZARD_DROP_PER_CLEAR));
            this.state = WIPE_CLEARED;
            return gain;
        }
    }

    clear_sets() {
        let num_cleared = 0;
        for (let row = 0; row < this.height; row++) {
            for (let col = 0; col < this.width; col++) {
                if (!IS_COLOR(this.data[row][col].type)) continue;
                if (this.check_set_from(row, col, this.data[row][col].type) >= CLEAR_MIN) {
                    num_cleared += this.clear_set_from(row, col, this.data[row][col].type);
                }
            }
        }
        this.add_score(num_cleared);
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
        this.data[row][col] = new Tile(TILE_CLEARED);
        this.data[row][col].start(); // ?
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
                this.speed++;
            } else {
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
                this.data[0][col] = new Tile();
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
        this.field = new Field();
        this.last_time = 0;
        this.key_delay = new Array(NUM_KEYS).fill(0);
        this.key_handled = new Array(NUM_KEYS).fill(false);
        this.paused = false;

        // Pixi setup
        this.app = new PIXI.Application({ width: 800, height: 600, backgroundColor: 0x000000 });
        document.body.appendChild(this.app.view);

        this.tile_size = 32;
        this.field_container = new PIXI.Container();
        this.app.stage.addChild(this.field_container);

        this.textures = {};
        this.textures_loaded = false;

        this.setup_input();
        this.app.ticker.add(this.update.bind(this));

        this.init();
    }

    async init() {
        await this.load_textures();
    }

    async load_textures() {
        const icon_names = ['cherry', 'coin', 'diamond', 'lemon', 'seven'];
        for (let i = 0; i < NUM_TYPES; i++) {
            this.textures[i + 1] = await PIXI.Assets.load(`icons/${icon_names[i]}.png`);
        }
        this.textures_loaded = true;
    }

    setup_input() {
        window.addEventListener('keydown', (e) => {
            const key_map = {
                'ArrowDown': KEY_DOWN,
                'ArrowLeft': KEY_LEFT,
                'ArrowRight': KEY_RIGHT,
                'KeyZ': KEY_ROTATE_LEFT,
                'KeyX': KEY_ROTATE_RIGHT,
                'Space': KEY_DROP
            };
            const key = key_map[e.code];
            if (key !== undefined) {
                e.preventDefault();
                this.key_delay[key] = performance.now();
                this.key_handled[key] = false;
            }
        });

        window.addEventListener('keyup', (e) => {
            const key_map = {
                'ArrowDown': KEY_DOWN,
                'ArrowLeft': KEY_LEFT,
                'ArrowRight': KEY_RIGHT,
                'KeyZ': KEY_ROTATE_LEFT,
                'KeyX': KEY_ROTATE_RIGHT,
                'Space': KEY_DROP
            };
            const key = key_map[e.code];
            if (key !== undefined) {
                this.key_delay[key] = 0;
            }
        });
    }

    update() {
        const now = performance.now();
        const ms = now - this.last_time;
        this.last_time = now;

        if (this.field.game_over()) return;

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
        document.getElementById('score').textContent = this.field.get_score();
        document.getElementById('speed').textContent = this.field.get_speed();
        document.getElementById('combo').textContent = this.field.get_best_combo();

        // Render
        this.render();
    }

    render() {
        if (!this.textures_loaded) return;

        this.field_container.removeChildren();

        // Render field
        for (let row = 0; row < this.field.height; row++) {
            for (let col = 0; col < this.field.width; col++) {
                const tile = this.field.get_tile(row, col);
                if (IS_COLOR(tile.type)) {
                    const sprite = new PIXI.Sprite(this.textures[tile.type]);
                    sprite.x = col * this.tile_size;
                    sprite.y = (this.field.height - 1 - row) * this.tile_size;
                    sprite.width = this.tile_size;
                    sprite.height = this.tile_size;
                    this.field_container.addChild(sprite);
                }
            }
        }

        // Render current block
        if (this.field.current) {
            for (let i = 0; i < BLOCK_SIZE; i++) {
                const row = this.field.current.get_row(i);
                const col = this.field.current.get_col(i);
                if (ON_FIELD(row, col, this.field.height, this.field.width)) {
                    const type = this.field.current.get_tile(i).type;
                    if (IS_COLOR(type)) {
                        const sprite = new PIXI.Sprite(this.textures[type]);
                        sprite.x = col * this.tile_size;
                        sprite.y = (this.field.height - 1 - row) * this.tile_size;
                        sprite.width = this.tile_size;
                        sprite.height = this.tile_size;
                        this.field_container.addChild(sprite);
                    }
                }
            }
        }
    }
}

// Start game
new Game();