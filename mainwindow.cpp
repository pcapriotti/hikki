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

#include "mainwindow.h"

#include <QStackedWidget>
#include <QWebView>
#include <KAction>
#include <KActionCollection>
#include <QDebug>
#include <KLocalizedString>
#include <KTextEditor/EditorChooser>
#include <KTextEditor/ConfigInterface>
#include <KTextEditor/View>
#include <KXMLGUIFactory>
#include <QApplication>

#include "webkitpart.h"

using namespace KTextEditor;

MainWindow::MainWindow(const QString& base)
: KParts::MainWindow(0,
  static_cast<Qt::WindowFlags>(KDE_DEFAULT_WINDOWFLAGS))
{
  
    
  QPalette palette = qApp->palette();
  //palette.setBrush(QPalette::Base, Qt::white);
  palette.setBrush(QPalette::Text, Qt::white);
  palette.setBrush(QPalette::WindowText, Qt::white);
  qApp->setPalette(palette);
  
  m_stack = new QStackedWidget(this);
  setCentralWidget(m_stack);
  
  m_notebook = new Notebook(base);
  m_note = m_notebook->index();
  kDebug() << "note path" << m_note->path();
  
  Editor* editor = EditorChooser::editor();
  Document* doc = editor->createDocument(this);
  m_editor = qobject_cast<View*>(doc->createView(this));
  if (KTextEditor::ConfigInterface *iface = 
    qobject_cast<KTextEditor::ConfigInterface*>(m_editor))
  {
    kDebug() << "Setting word wrap";
    iface->setConfigValue("dynamic-word-wrap", true);
  }
      

  m_stack->addWidget(m_editor);

  m_browser = new WebKitPart(this, m_notebook);
  m_stack->addWidget(m_browser->widget());
  connect(m_browser, SIGNAL(noteChanged(QString)),
          this, SLOT(noteChanged(QString)));
  
  setupActions();
  setupGUI();
  removeUnwantedActions();

  displayMode();
}

MainWindow::~MainWindow()
{
  delete m_notebook;
}

void MainWindow::setupActions()
{
  KAction* editMode = new KAction(
    KIcon("document-edit"),
    i18n("&Edit mode"),
    this);
  actionCollection()->addAction("edit_mode", editMode);
  connect(editMode, SIGNAL(triggered()),
          this, SLOT(editMode()));
          
  KAction* displayMode = new KAction(
    KIcon("document-preview"),
    i18n("&Display mode"),
    this);
  actionCollection()->addAction("display_mode", displayMode);
  connect(displayMode, SIGNAL(triggered()),
          this, SLOT(displayMode()));
          
  actionCollection()->addAction("go_back",
    m_browser->view()->pageAction(QWebPage::Back));
  actionCollection()->addAction("go_forward",
    m_browser->view()->pageAction(QWebPage::Forward));
}

void MainWindow::removeUnwantedActions()
{
  delete m_editor->action("file_save");
  delete m_editor->action("file_save_as");
}

void MainWindow::editMode()
{
  refreshEditor();
  m_stack->setCurrentWidget(m_editor);
  guiFactory()->addClient(m_editor);
  m_mode = EditMode;
}

void MainWindow::refreshEditor()
{
  Document* doc = m_editor->document();
  if (doc) {
    if (doc->url().isValid())
      doc->save();
    kDebug() << m_note->path();
    KUrl url("file://" + m_note->path());
    
    if (doc->url() != url) {
      kDebug() << url;
      doc->openUrl(url);
      doc->setHighlightingMode("markdown");
    }
  }
}

void MainWindow::displayMode()
{
  displayCurrentNote();
  m_stack->setCurrentWidget(m_browser->widget());
  guiFactory()->removeClient(m_editor);
  m_mode = DisplayMode;
}

void MainWindow::displayCurrentNote()
{
  kDebug() << m_note->name();
  Q_ASSERT(m_note);
  if (m_editor->document()->url().isValid())
    m_editor->document()->save();
  displayNote(m_note);
}

void MainWindow::displayNote(NotePtr note)
{
  QString path = note->rendered();
  m_browser->openNote(path);
}

void MainWindow::noteChanged(const QString& path)
{
  kDebug() << path;
  m_note = m_notebook->openFromRendered(path);
  if (m_mode == EditMode)
    editMode();
}
