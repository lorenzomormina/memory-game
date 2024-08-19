appName = "memory"
winTitle = "Memory"
winSize = {800, 600}
bgColor = {150, 150, 150}
frameRate = 60

boardMargin = 10
cardsMargin = 10
timerFlipback = 0.5
selectedScale = 1.2

scoreLabel = {
    format = "Pairs left: %d. Attempts: %d.",
    fontSize = 14,
    color = {255, 255, 255},
    posX = 20,
    marginTop = 10
}

btnReset = {
    text = "Reset",
    fontSize = 14,
    color = {255, 255, 255},
    bgColor = {64, 64, 64},
    size = {100, 30},
    marginTop = 10,
    posX = 20
}

confPrompt = {
    size = {400, 150},
    bgColor = {128, 128, 128},
    label = {
        text = "Are you sure you want to reset the game?",
        pos = {20, 20},
        fontSize = 14,
        color = {255, 255, 255}
    },
    btnYes = {
        text = "Yes",
        pos = {20, 70},
        fontSize = 14,
        color = {255, 255, 255},
        bgColor = {64, 64, 64},
        size = {100, 30}
    },
    btnNo = {
        text = "No",
        pos = {150, 70},
        fontSize = 14,
        color = {255, 255, 255},
        bgColor = {64, 64, 64},
        size = {100, 30}
    }
}

console = {
    pos = {20, 495},
    size = {760, 90},
    fontSize = 14,
    color = {0, 0, 0},
    fontColor = {255, 255, 255},
    prompt = "> "
}


test = { {a = 3, b = 1}, {a = 2, b = 2}, {a = 1, b = 3} }
global_var = {nested = {value = 42, arr = {5, 6, 7}}}