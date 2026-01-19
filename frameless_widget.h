#ifndef FRAMELESS_WIDGET_H
#define FRAMELESS_WIDGET_H

#include <QDialog>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QMouseEvent>
#include <QPoint>

class FramelessWidget : public QDialog
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
    bool CheckScalePos(QPoint);

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
    QPoint m_scale_pos;
    bool m_drag;
    bool m_scale;
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

bool FramelessWidget::CheckScalePos(QPoint pos)
{
    QRect rect = this->geometry();
    QPoint right_bottom_corner = QPoint(rect.x() + rect.width(), rect.y() + rect.height());

    int h_away = right_bottom_corner.x() - pos.x();
    int v_away = right_bottom_corner.y() - pos.y();

    return h_away >= 0 && h_away <= 10 &&
           v_away >= 0 && v_away <= 10;
}

FramelessWidget::FramelessWidget(QString title,
                                 QString app_icon, QString min_icon, QString max_icon, QString normal_icon, QString close_icon,
                                 QWidget *parent) :
    QDialog(parent), m_title(title), m_app_icon(app_icon), m_min_icon(min_icon), m_max_icon(max_icon), m_normal_icon(normal_icon), m_close_icon(close_icon)
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
    m_scale = false;
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
    QPoint global_pos = event->globalPos();
    if(CheckDragPos(pos))
    {
        m_drag = true;
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
        m_drag_pos = event->globalPosition().toPoint() - this->pos();
#else
        m_drag_pos = event->globalPos() - this->pos();
#endif
    }
    else if (CheckScalePos(global_pos))
    {
        QRect rect = this->geometry();
        QPoint right_bottom_corner = QPoint(rect.x() + rect.width(), rect.y() + rect.height());

        m_scale_pos = QPoint(right_bottom_corner.x() - global_pos.x(), right_bottom_corner.y() - global_pos.y());
        m_scale = true;
    }

    event->accept();
}

inline void FramelessWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (m_drag)
    {
        if (m_show_state == ShowState::max)
        {
            this->showNormal();
            m_show_state = ShowState::normal;
            m_btn_max->setIcon(QIcon(m_max_icon));
        }
        else
        {
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
            QPoint newPos = event->globalPosition().toPoint() - m_drag_pos;
#else
            QPoint newPos = event->globalPos() - m_drag_pos;
#endif
            this->move(newPos);
        }
    }
    else if (m_scale)
    {
        QPoint new_right_bottom_corner = event->globalPos() + m_scale_pos;
        QPoint cur_pos = this->pos();
        this->resize(new_right_bottom_corner.x() - cur_pos.x(), new_right_bottom_corner.y() - cur_pos.y());
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
    this->setWindowIcon(QIcon(m_app_icon));
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
