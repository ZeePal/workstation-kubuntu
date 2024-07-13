require("todo-comments").setup({
    highlight = {
        comments_only = false,
        pattern = [[<(KEYWORDS)>]], -- Vim Regex
        keyword = "bg", -- No "Wide" plz
    },
    search = {
        pattern = [[\b(KEYWORDS)\b]],  -- RipGrep Regex
    },
})
