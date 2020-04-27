#ifndef EDITORMAINFRAME_H
#define EDITORMAINFRAME_H

#include <QMainWindow>

class EditorMainFrame : public QMainWindow
{
    Q_OBJECT

public:
    EditorMainFrame(QWidget *parent = nullptr);
    ~EditorMainFrame();
};
#endif // EDITORMAINFRAME_H
