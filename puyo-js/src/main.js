import './game.js';

// Start the game
(async () => {
    const game = new Game();
    await game.init_app();
})();