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
#include <QWebHistory>
#include <KAction>
#include <KActionCollection>
#include <QDebug>
#include <KLocalizedString>
#include <KMessageBox>
#include <KTextEditor/EditorChooser>
#include <KTextEditor/ConfigInterface>
#include <KTextEditor/View>
#include <KXMLGUIFactory>
#include <QApplication>

#include "webkitpart.h"
#include "proxyaction.h"

using namespace KTextEditor;

MainWindow::MainWindow(const QString& base)
: KParts::MainWindow(0,
  static_cast<Qt::WindowFlags>(KDE_DEFAULT_WINDOWFLAGS))
{  
  m_stack = new QStackedWidget(this);
  setCentralWidget(m_stack);
  
  m_notebook = new Notebook(base);
  
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

  m_note = m_notebook->index();
  m_browser->openNote(m_note);
  displayMode();
}

MainWindow::~MainWindow()
{
  delete m_notebook;
}

void MainWindow::setupActions()
{
  {
    KAction* editMode = new KAction(
      KIcon("document-edit"),
      i18n("&Edit mode"),
      this);
    actionCollection()->addAction("edit_mode", editMode);
    connect(editMode, SIGNAL(triggered()),
            this, SLOT(editMode()));
  }
   
  {
    KAction* displayMode = new KAction(
      KIcon("document-preview"),
      i18n("&Display mode"),
      this);
    actionCollection()->addAction("display_mode", displayMode);
    connect(displayMode, SIGNAL(triggered()),
            this, SLOT(displayMode()));
  }
  
  {
    QAction* webBack = m_browser->view()->pageAction(QWebPage::Back);
    KAction* back = new ProxyAction(webBack, this);
    actionCollection()->addAction("back", back);
  }
  
  {
    QAction* webForward = m_browser->view()->pageAction(QWebPage::Forward);
    KAction* forward = new ProxyAction(webForward, this);
    actionCollection()->addAction("forward", forward);
  }
  
  KStandardAction::home(this, SLOT(index()), actionCollection());
  
  {
    KAction* deleteAction = new KAction(
      KIcon("list-remove"),
      i18n("&Delete note"),
      this);
    actionCollection()->addAction("delete_note", deleteAction);
    connect(deleteAction, SIGNAL(triggered()),
            this, SLOT(deleteNote()));
  }
}

void MainWindow::deleteNote()
{
  if (m_note)
  {
    int resp = KMessageBox::warningYesNo(this,
      i18n("Are you sure you want to delete note <em>%1</em>?",
            m_note->name()),
      i18n("Confirm delete"));
    if (resp == KMessageBox::Yes)
    {
      if (m_editor->document()->url().isValid())
        m_editor->document()->closeUrl();
      m_note->remove();
      m_browser->view()->page()->history()->back();
    }
  }
}

void MainWindow::index()
{
  m_note = m_notebook->open("index");
  m_browser->openNote(m_note);
  if (m_mode == EditMode)
    editMode();
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
  // the current note is already loaded in the browser at this point
  
  // save note
  if (m_editor->document()->url().isValid())
    m_editor->document()->save();
  
  // rerender and redisplay note
  m_browser->refresh();
  
  // show browser
  m_stack->setCurrentWidget(m_browser->widget());
  
  // remove editor GUI
  guiFactory()->removeClient(m_editor);
  
  // update mode
  m_mode = DisplayMode;
}

void MainWindow::renderCurrentNote()
{
  Q_ASSERT(m_note);
}

void MainWindow::noteChanged(const QString& path)
{
  m_note = m_notebook->openFromRendered(path);
  if (m_mode == EditMode)
    editMode();
}
