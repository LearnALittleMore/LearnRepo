#ifndef FRAMELESS_WIDGET_H
#define FRAMELESS_WIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QMouseEvent>
#include <QPoint>

class FramelessWidget : public QWidget
{
public:
    explicit FramelessWidget(QString title,
                             QString app_icon, QString min_icon, QString max_icon, QString normal_icon, QString close_icon,
                             QWidget *parent = nullptr);

    virtual ~FramelessWidget();

    void SetCentralWidget(QWidget *w);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    void InitUI();
    void InitStyleSheet();
    bool CheckDragPos(QPoint);

private slots:
    void on_btn_min_clicked();
    void on_btn_max_clicked();
    void on_btn_close_clicked();

private:
    QVBoxLayout *m_layout;
    QHBoxLayout *m_layout_2;
    QPushButton *m_btn_min;
    QPushButton *m_btn_max;
    QPushButton *m_btn_close;
    QLabel *m_label_icon;
    QLabel *m_label_title;
    QSpacerItem *m_spacer;
    QPoint m_drag_pos;
    bool m_drag;
    QString m_title, m_app_icon, m_min_icon, m_max_icon, m_normal_icon, m_close_icon, m_style_sheet;
    enum class ShowState {max, normal} m_show_state;
};

//style sheet
void FramelessWidget::InitStyleSheet()
{
    m_style_sheet =
    R"(
        #frameless_widget {
            background-color:rgb(255,255,255);
        }

        #label_icon {
            min-height:25px;
            max-height:25px;
        }

        QPushButton {
            border:none;
            min-height:25px;
            min-width:27px;
            background-color:rgb(255,255,255,0);
        }

        #btn_min:hover,#btn_max:hover {
            background-color:rgb(229,229,229);
        }

        #btn_close:hover {
            background-color:rgb(255,0,0);
        }
    )";
}

inline bool FramelessWidget::CheckDragPos(QPoint pos)
{
    return m_spacer->geometry().contains(pos);
}

FramelessWidget::FramelessWidget(QString title,
                                 QString app_icon, QString min_icon, QString max_icon, QString normal_icon, QString close_icon,
                                 QWidget *parent) :
    QWidget(parent), m_title(title), m_app_icon(app_icon), m_min_icon(min_icon), m_max_icon(max_icon), m_normal_icon(normal_icon), m_close_icon(close_icon)
{
    m_layout = new QVBoxLayout(this);
    m_layout_2 = new QHBoxLayout(this);
    m_btn_min = new QPushButton(this);
    m_btn_max = new QPushButton(this);
    m_btn_close = new QPushButton(this);
    m_label_icon = new QLabel(this);
    m_label_title = new QLabel(this);
    m_spacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Preferred);
    m_drag = false;
    m_show_state = ShowState::normal;

    InitStyleSheet();
    InitUI();
}

FramelessWidget::~FramelessWidget()
{

}

void FramelessWidget::SetCentralWidget(QWidget *w)
{
    m_layout->addWidget(w);
    w->setParent(this);
}

inline void FramelessWidget::mousePressEvent(QMouseEvent *event)
{
    QPoint pos = event->pos();
    if(CheckDragPos(pos))
    {
        m_drag = true;
        m_drag_pos = event->globalPosition().toPoint() - this->pos();
    }

    event->accept();
}

inline void FramelessWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (m_drag)
    {
        QPoint newPos = event->globalPosition().toPoint() - m_drag_pos;
        this->move(newPos);
    }

    event->accept();
}

inline void FramelessWidget::mouseReleaseEvent(QMouseEvent *event)
{
    m_drag = false;
    event->accept();
}

void FramelessWidget::InitUI()
{
    m_label_icon->setObjectName("label_icon");
    m_label_title->setText(m_title);
    m_label_title->setObjectName("label_title");
    m_btn_min->setObjectName("btn_min");
    m_btn_max->setObjectName("btn_max");
    m_btn_close->setObjectName("btn_close");

    m_layout_2->addWidget(m_label_icon);
    m_layout_2->addWidget(m_label_title);
    m_layout_2->addItem(m_spacer);
    m_layout_2->addWidget(m_btn_min);
    m_layout_2->addWidget(m_btn_max);
    m_layout_2->addWidget(m_btn_close);
    m_layout_2->setContentsMargins(4,0,0,0);

    m_layout->addLayout(m_layout_2);
    m_layout->setSpacing(0);
    m_layout->setContentsMargins(0,0,0,0);

    m_label_icon->setPixmap(QPixmap(m_app_icon));
    m_btn_min->setIcon(QIcon(m_min_icon));
    m_btn_max->setIcon(QIcon(m_max_icon));
    m_btn_close->setIcon(QIcon(m_close_icon));

    connect(m_btn_min, &QPushButton::clicked, this, &FramelessWidget::on_btn_min_clicked);
    connect(m_btn_max, &QPushButton::clicked, this, &FramelessWidget::on_btn_max_clicked);
    connect(m_btn_close, &QPushButton::clicked, this, &FramelessWidget::on_btn_close_clicked);

    this->setObjectName("frameless_widget");
    this->setLayout(m_layout);
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setStyleSheet(m_style_sheet);
}

void FramelessWidget::on_btn_min_clicked()
{
    this->showMinimized();
}

void FramelessWidget::on_btn_max_clicked()
{
    if (m_show_state == ShowState::normal)
    {
        this->showMaximized();
        m_show_state = ShowState::max;
        m_btn_max->setIcon(QIcon(m_normal_icon));
    }
    else
    {
        this->showNormal();
        m_show_state = ShowState::normal;
        m_btn_max->setIcon(QIcon(m_max_icon));
    }
}

void FramelessWidget::on_btn_close_clicked()
{
    this->close();
}

#endif // FRAMELESS_WIDGET_H
