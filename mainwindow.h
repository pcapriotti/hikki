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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QList>
#include <QWidget>
#include <KParts/MainWindow>
#include "notebook.h"
#include "note.h"

namespace KTextEditor { class View; }
class WebKitPart;
class QStackedWidget;
class Notebook;

class MainWindow : public KParts::MainWindow
{
Q_OBJECT
  enum Mode
  {
    DisplayMode,
    EditMode
  };

  void setupActions();
  void removeUnwantedActions();

  Notebook* m_notebook;
  NotePtr m_note;
  
  QStackedWidget* m_stack;
  KTextEditor::View* m_editor;
  WebKitPart* m_browser;
  
  Mode m_mode;
  
  void refreshEditor();
public:
  MainWindow(const QString& base);
  ~MainWindow();
public slots:
  void editMode();
  void displayMode();
  void index();
  void deleteNote();
  
  void renderCurrentNote();
  void noteChanged(const QString& path);
};

#endif // MAINWINDOW_H
