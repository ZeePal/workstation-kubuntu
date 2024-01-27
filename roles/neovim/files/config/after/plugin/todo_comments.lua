require("todo-comments").setup({
    highlight = {
        pattern = [[<(KEYWORDS)>]],
    },
    search = {
        pattern = [[\b(KEYWORDS)\b]],
    },
})
