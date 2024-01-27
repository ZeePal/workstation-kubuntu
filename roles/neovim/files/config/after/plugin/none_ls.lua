local null_ls = require("null-ls")
local augroup = vim.api.nvim_create_augroup("LspFormatting", {})

null_ls.setup({
    -- you can reuse a shared lspconfig on_attach callback here
    on_attach = function(client, bufnr)
        if client.supports_method("textDocument/formatting") then
            vim.api.nvim_clear_autocmds({ group = augroup, buffer = bufnr })
            vim.api.nvim_create_autocmd("BufWritePre", {
                group = augroup,
                buffer = bufnr,
                callback = function()
                    vim.lsp.buf.format({ async = false })
                end,
            })
        end
    end,

    sources = {
        null_ls.builtins.formatting.black,
        null_ls.builtins.formatting.gofmt,
        null_ls.builtins.formatting.goimports,
        null_ls.builtins.formatting.isort,
        --null_ls.builtins.formatting.json_tool,
        --null_ls.builtins.formatting.markdownlint,
        null_ls.builtins.formatting.prettierd,
        --null_ls.builtins.formatting.rustfmt,
        null_ls.builtins.formatting.shfmt,
        null_ls.builtins.formatting.terraform_fmt,
        --null_ls.builtins.formatting.yamlfmt,

        --null_ls.builtins.diagnostics.ansiblelint,
        --null_ls.builtins.diagnostics.bandit,
        --null_ls.builtins.diagnostics.jsonlint,
        --null_ls.builtins.diagnostics.markdownlint,
        --null_ls.builtins.diagnostics.yamllint,
    },
})
