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

#ifndef WEBKITPART_H
#define WEBKITPART_H

#include <KParts/Part>
#include <QWebPage>
#include <QNetworkAccessManager>

#include "notebook.h"

class QWebView;

class WebKitPart : public KParts::ReadOnlyPart
{
  Q_OBJECT
  QWebView* m_view;
public:
  WebKitPart(QWidget* parent, Notebook* notebook);
  
  virtual bool openUrl(const KUrl& url);
  virtual bool openNote(const QString& path);
  virtual bool openFile();
  
  virtual KUrl url() const;
  QWebView* view();
private slots:
  void onUrlChanged(const QUrl& url);
signals:
  void noteChanged(const QString& path);
};

class WebPage : public QWebPage
{
  Q_OBJECT
public:
  WebPage(QObject* parent, Notebook* notebook);
protected:
  virtual bool acceptNavigationRequest(QWebFrame* frame,
                                       const QNetworkRequest& req,
                                       NavigationType type);
};

class AccessManager : public QNetworkAccessManager
{
  Q_OBJECT
  Notebook* m_notebook;
public:
  AccessManager(QObject* parent, Notebook* notebook);
  virtual QNetworkReply* createRequest(
      Operation op,
      const QNetworkRequest& req,
      QIODevice* outgoingData = 0);
private:
  QString relativePath(const QNetworkRequest& req);
signals:
  void noSuchNote(NotePtr note);
  void loading(NotePtr note);
};

#endif // WEBKITPART_H
