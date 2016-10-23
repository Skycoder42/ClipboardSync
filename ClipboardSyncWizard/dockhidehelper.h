#ifndef DOCKHIDEHELPER_H
#define DOCKHIDEHELPER_H

#include <QObject>

class DockHideHelper : public QObject
{
    Q_OBJECT
public:
    explicit DockHideHelper(QObject *parent = nullptr);

    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    bool isHidden;

    void setDockIconStyle(bool hidden);
};

#endif // DOCKHIDEHELPER_H
