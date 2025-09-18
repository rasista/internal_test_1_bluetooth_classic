# 📘 Lite Controller – Doxygen Documentation Guide

This project uses **Doxygen** to build API‑level documentation for the Lite Controller module.

---

## 1 🔧 Prerequisites

| Requirement | Suggested command (Ubuntu/Debian) |
|-------------|-----------------------------------|
| Doxygen ≥ 1.9.4 | `sudo apt update && sudo apt install doxygen` |
| C / C++ toolchain (for source code) | Already required by the project |

> **Check your version**  
> ```bash
> doxygen --version    # should print 1.9.4 or newer
> ```

---

## 2 💻 Installation Steps (one‑time)

1. **Clone or pull** the repository containing the module.  
2. **Verify** the two documentation files are present:  

```bash
ls -la Doxyfile DOXYGEN_GUIDE.md
```

You should see both files listed:
- `Doxyfile` # configuration
- `DOXYGEN_GUIDE.md` # this guide

3. **(Optional)** Add the generated docs directory to your `.gitignore`:  

```bash
echo "lite_controller_docs/" >> .gitignore
```

---

## 3 🚀 Generate HTML Documentation

Run the following from the directory that contains `Doxyfile`:

```bash
doxygen Doxyfile
```

Doxygen will:
- Parse all source files listed in the INPUT variable inside Doxyfile
- Emit HTML output into `lite_controller_docs/html/` (or whatever folder is set in OUTPUT_DIRECTORY)

---

## 4 📂 View the Documentation

Open the main page in your browser:

```bash
xdg-open lite_controller_docs/html/index.html
```

Or navigate manually to `lite_controller_docs/html/index.html` and double‑click it.

---

## 5 🔄 Regenerating Documentation

To update the documentation after making changes to source code:

1. **Modify** your source files with proper Doxygen comments
2. **Run** the generation command again:
   ```bash
   doxygen Doxyfile
   ```
3. **Refresh** your browser to see the updated documentation

---

## 6 ⚙️ Configuration

The `Doxyfile` contains all configuration options. Key settings include:

- `INPUT`: Source files to document
- `OUTPUT_DIRECTORY`: Where to place generated docs
- `GENERATE_HTML`: Enable HTML output
- `EXTRACT_ALL`: Extract undocumented entities
- `EXTRACT_PRIVATE`: Extract private members

To modify settings, edit `Doxyfile` and regenerate documentation.

---

## 7 🐛 Troubleshooting

### Common Issues

**Doxygen not found:**
```bash
sudo apt update && sudo apt install doxygen
```

**Permission denied:**
```bash
chmod +x lite_controller_docs/html/index.html
```

**No output generated:**
- Check that `Doxyfile` exists in current directory
- Verify source files listed in `INPUT` exist
- Check console output for error messages

**Documentation not updating:**
- Clear the output directory: `rm -rf lite_controller_docs/`
- Regenerate: `doxygen Doxyfile`

---

## 8 🎯 Quick Start Summary

```bash
# 1. Install Doxygen
sudo apt update && sudo apt install doxygen

# 2. Verify files exist
ls -la Doxyfile DOXYGEN_GUIDE.md

# 3. Generate documentation
doxygen Doxyfile

# 4. View documentation
xdg-open lite_controller_docs/html/index.html
```

That's it! Your API documentation is now generated and ready to view. 🎉
