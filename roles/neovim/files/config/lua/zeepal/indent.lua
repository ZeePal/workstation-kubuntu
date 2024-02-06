vim.api.nvim_create_augroup("SetIndent", { clear = true })
vim.api.nvim_create_autocmd("FileType", {
    group = "SetIndent",
    pattern = {
        "terraform",
    },
    callback = function()
        vim.opt.tabstop = 2
        vim.opt.softtabstop = 2
        vim.opt.shiftwidth = 2
    end
})
