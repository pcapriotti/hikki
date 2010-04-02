// Copyright (c) 2010  Paolo Capriotti <p.capriotti@gmail.com>
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
// 
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
// 
// THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
// IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
// OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
// IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
// NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
// THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "notebook.h"
#include "note.h"

#include <KDebug>

Notebook::Notebook(const QString& base)
: m_base(base)
{
  m_header = "<html><head>"
        "<link rel=\"stylesheet\" href=\"hikki://data/example.css\" "
        "type=\"text/css\"></link>"
        "<link rel=\"stylesheet\" href=\"hikki://data/prettify.css\" "
        "type=\"text/css\"></link>"
        "<script type=\"text/javascript\" src=\"hikki://data/prettify.js\">"
        "</script>"
        "</head><body onload=\"prettyPrint()\">";
  m_footer = "</body></html>";
  
  if (!m_base.exists())
    kError() << "Invalid notebook directory" << base;
}

NotePtr Notebook::open(const QString& note)
{
  return NotePtr(new Note(note, this));
}

NotePtr Notebook::index()
{
  return open("index");
}

QString Notebook::notePath(const QString& note) const
{
  return m_base.absoluteFilePath(note);
}

NotePtr Notebook::openFromRendered(const QString& renderedFile)
{
  QString rel = m_base.relativeFilePath(renderedFile);
  QString note = QFileInfo(rel).baseName();
  return open(note);
}

QString Notebook::renderedNotePath(const QString& note)
{
  QString path = QFileInfo(m_render_dir.name(), note).absoluteFilePath();
  return path + ".html";
}

QString Notebook::saveRenderedNote(const QString& note,
                                const QString& content)
{
  QString path = renderedNotePath(note);
  QFile file(path);
  if (file.open(QIODevice::WriteOnly)) {
    file.write(m_header.toUtf8());
    file.write(content.toUtf8());
    file.write(m_footer.toUtf8());
    return path;
  }
  else {
    return QString();
  }
}
