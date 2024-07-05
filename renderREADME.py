import markdown
import pdfkit

# Read the markdown file
with open('README.md', 'r') as f:
    text = f.read()

# Convert markdown to HTML
html = markdown.markdown(text)

# Convert HTML to PDF
pdfkit.from_string(html, 'README.pdf')