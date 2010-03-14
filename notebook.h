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

#ifndef NOTEBOOK_H
#define NOTEBOOK_H

#include <QDir>
#include <QSharedPointer>
#include <KUrl>
#include <KTempDir>

class Note;
typedef QSharedPointer<Note> NotePtr;

class Notebook
{
  QDir m_base;
  KTempDir m_render_dir;
  QString m_header;
  QString m_footer;
public:
  Notebook(const QString& base);
  
  NotePtr open(const QString& path);
  NotePtr openFromRendered(const QString& renderedFile);
  NotePtr index();
  
  QString notePath(const QString& note) const;
  QString saveRenderedNote(const QString& note,
                           const QString& content);
  QString renderedNotePath(const QString& note);
};

#endif // NOTEBOOK_H
