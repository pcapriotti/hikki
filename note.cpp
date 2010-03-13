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

#include "note.h"

#include <KDebug>

#include "notebook.h"

extern "C" {
#include "markdown/mkdio.h"
}

static int qfile_getc(void* ctx)
{
  char c;
  QFile* file = reinterpret_cast<QFile*>(ctx);
  if (file->getChar(&c))
    return static_cast<int>(c);
  else
    return EOF;
}

class AutoMMIOT
{
  MMIOT* m_doc;
public:
  AutoMMIOT(MMIOT* doc) : m_doc(doc) { }
  virtual ~AutoMMIOT() { mkd_cleanup(m_doc); }
};

Note::Note(const QString& name, Notebook* parent)
: m_name(name)
, m_parent(parent)
{
  if (!exists()) {
    QFile file(path());
    if (file.open(QIODevice::WriteOnly)) {
      QString content = "# " + name + "\n";
      file.write(content.toUtf8());
    }
  }
}

QString Note::path() const
{
  return m_parent->notePath(m_name);
}

QString Note::rendered()
{
  QFile file(path());
  if (!file.open(QIODevice::ReadOnly))
    return QString();
  
  // convert to markdown, put the MMIOT object
  // in a smart pointer, so that it is automatically
  // cleaned up when the scope exits
  MMIOT* doc = mkd_general(
      qfile_getc,
      reinterpret_cast<QFile*>(&file),
      0);
  AutoMMIOT guard(doc);

  QString result;
  if (mkd_compile(doc, 0)) {
    char* res;
    int size = mkd_document(doc, &res);
    if (size != 0) {
      result = QString::fromUtf8(res, size);
    }
  }
  if (result.isEmpty())
    return QString();
  
  QString renderedPath = m_parent->saveRenderedNote(m_name, result);
  
  return renderedPath;
}

bool Note::exists() const
{
  return QFileInfo(path()).exists();
}

QString Note::name() const
{
  return m_name;
}

