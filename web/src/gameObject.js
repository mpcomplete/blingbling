export class Animation {
    constructor(duration, tickFn) {
        this.age = 0;
        this.duration = duration;
        this.tickFn = tickFn;
    }
    tick(ms) {
        this.age += ms;
        this.done = this.tickFn(this.age / this.duration);
        this.done = this.done || this.age >= this.duration;
        return this.done;
    }
}

export class GameObject {
    constructor() {
        this.animations = [];
    }

    tick(ms) {
        this.animations = this.animations.filter(a => !a.tick(ms));
    }
}
