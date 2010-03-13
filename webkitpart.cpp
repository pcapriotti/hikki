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

#include "webkitpart.h"
#include "notebook.h"
#include "note.h"

#include <KDebug>
#include <KRun>
#include <KStandardDirs>
#include <QWebView>
#include <QNetworkRequest>

WebKitPart::WebKitPart(QWidget* parent, Notebook* notebook)
: KParts::ReadOnlyPart(parent)
{
  m_view = new QWebView(parent);
  m_view->setPage(new WebPage(this, notebook));
  setWidget(m_view);
  
  QPalette palette = m_view->palette();
  palette.setBrush(QPalette::Base, Qt::transparent);
  m_view->page()->setPalette(palette);
  m_view->setAttribute(Qt::WA_OpaquePaintEvent, false);

  connect(m_view, SIGNAL(urlChanged(QUrl)),
          this, SLOT(onUrlChanged(QUrl)));
}

QWebView* WebKitPart::view()
{
  return m_view;
}

bool WebKitPart::openUrl(const KUrl& url)
{
  m_view->load(url);
  return true;
}

bool WebKitPart::openNote(const QString& path)
{
  return openUrl(KUrl("hikki://note/" + path));
}

bool WebKitPart::openFile()
{
  return false;
}

KUrl WebKitPart::url() const
{
  return m_view->url();
}

void WebKitPart::onUrlChanged(const QUrl& url)
{
  if (url.scheme() == "hikki") {
    if (url.host() == "note") {
      QString path = url.path();
      if (path[0] == '/')
        path = path.mid(1);
      emit noteChanged(path);
    }
  }
}

////

WebPage::WebPage(QObject* parent, Notebook* notebook)
: QWebPage(parent)
{
  setNetworkAccessManager(new AccessManager(this, notebook));
}

bool WebPage::acceptNavigationRequest(QWebFrame*,
                                      const QNetworkRequest& req,
                                      NavigationType)
{
  if (req.url().scheme() != "hikki") {
    new KRun(KUrl(req.url()), 0);
    return false;
  }
  return true;
}

///

AccessManager::AccessManager(QObject* parent, Notebook* notebook)
: QNetworkAccessManager(parent)
, m_notebook(notebook)
{ }

QNetworkReply* AccessManager::createRequest(
    Operation op,
    const QNetworkRequest& req,
    QIODevice* data)
{
  if (req.url().scheme() == "hikki") {
    if (req.url().host() == "note") {
      QString path = relativePath(req);
      NotePtr note = m_notebook->openFromRendered(path);
      if (note->exists()) {
        QNetworkRequest r(note->rendered());
        QNetworkReply* reply =
            QNetworkAccessManager::
              createRequest(op, r, data);
        emit loading(note);
        return reply;
      }
      else {
        emit noSuchNote(note);
      }
    }
    else if (req.url().host() == "data") {
      QString path = KStandardDirs::locate("appdata", relativePath(req));
      kDebug() << "data:" << path;
      if (!path.isEmpty())
      {
        QNetworkRequest r((QUrl(path)));
        return QNetworkAccessManager::createRequest(op, r, data);
      }
    }
  }  
  return QNetworkAccessManager::
      createRequest(op, req, data);
}

QString AccessManager::relativePath(const QNetworkRequest& req)
{
  QString path = req.url().path();
  if (path[0] == '/')
    path = path.mid(1);
  return path;
}
