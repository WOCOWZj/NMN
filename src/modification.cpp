#include "modification.h" // 包含对应的头文件

// Modification构造函数：创建音符修改对话框
Modification::Modification(const QString &title, QWidget *parent, const Note &note, const Mode &Mode)
    : QDialog(parent) // 调用父类构造函数
{
    this->setWindowTitle(title); // 设置对话框标题
    this->setModal(true);        // 设置为模态对话框

    // 创建布局管理器
    QVBoxLayout *vbox = new QVBoxLayout(this);  // 垂直布局
    QHBoxLayout *hbox1 = new QHBoxLayout(this); // 第一行水平布局
    QHBoxLayout *hbox2 = new QHBoxLayout(this); // 第二行水平布局
    QHBoxLayout *hbox3 = new QHBoxLayout(this); // 第三行水平布局
    QHBoxLayout *hbox4 = new QHBoxLayout(this); // 第四行水平布局

    // 创建音名标签和下拉框
    this->notename_label = new QLabel("音名");                                 // 音名标签
    this->notename_combobox = new QComboBox();                                 // 音名下拉框
    this->notename_combobox->addItems({"1", "2", "3", "4", "5", "6", "7"});    // 添加音名选项
    this->notename_combobox->setCurrentIndex(static_cast<int>(note.noteName)); // 设置当前选中项
    hbox1->addWidget(this->notename_label);                                    // 将标签添加到布局
    hbox1->addWidget(this->notename_combobox);                                 // 将下拉框添加到布局

    // 创建变音记号标签和下拉框
    this->accidental_label = new QLabel("升降调");                                 // 变音记号标签
    this->accidental_combobox = new QComboBox;                                     // 变音记号下拉框
    this->accidental_combobox->addItems({"Sharp", "Natural", "Flat"});             // 添加变音记号选项
    this->accidental_combobox->setCurrentIndex(static_cast<int>(note.accidental)); // 设置当前选中项
    hbox2->addWidget(this->accidental_label);                                      // 将标签添加到布局
    hbox2->addWidget(this->accidental_combobox);                                   // 将下拉框添加到布局

    // 创建八度标签和下拉框
    this->octdot_label = new QLabel("音区");                               // 八度标签
    this->octdot_combobox = new QComboBox;                                 // 八度下拉框
    this->octdot_combobox->addItems({"Lower", "None", "Higher"});          // 添加八度选项
    this->octdot_combobox->setCurrentIndex(static_cast<int>(note.octDot)); // 设置当前选中项
    hbox3->addWidget(this->octdot_label);                                  // 将标签添加到布局
    hbox3->addWidget(this->octdot_combobox);                               // 将下拉框添加到布局

    // 创建确认和取消按钮
    this->accept_button = new QPushButton("确定"); // 确认按钮
    this->cancel_button = new QPushButton("取消"); // 取消按钮
    hbox4->addStretch();                           // 添加弹性空间
    hbox4->addWidget(this->accept_button);         // 将确认按钮添加到布局
    hbox4->addWidget(this->cancel_button);         // 将取消按钮添加到布局

    // 将行布局添加到垂直布局
    vbox->addLayout(hbox1);    // 添加音名行
    vbox->addLayout(hbox2);    // 添加变音记号行
    if (Mode == Mode::Mod8App) // 如果是修改应用模式，则添加八度行
        vbox->addLayout(hbox3);
    vbox->addLayout(hbox4); // 添加按钮行

    // 连接信号槽：按钮点击事件连接到对应的槽函数
    connect(this->accept_button, &QPushButton::clicked, this, &Modification::onAccept);
    connect(this->cancel_button, &QPushButton::clicked, this, &Modification::onCancel);

    this->accept_button->setDefault(true); // 设置确认按钮为默认按钮
}

// exec方法：执行对话框并居中显示
int Modification::exec()
{
    // 将对话框居中显示在父窗口中
    if (this->parentWidget())
        this->move(this->parentWidget()->window()->frameGeometry().center() - this->frameGeometry().center());
    return QDialog::exec(); // 执行对话框
}

// onAccept槽：处理确认按钮点击事件
void Modification::onAccept()
{
    Note note; // 创建新的Note对象
    // 从下拉框获取选择的值并设置到Note对象
    note.noteName = static_cast<Note::NoteName>(this->notename_combobox->currentIndex());
    note.accidental = static_cast<Note::Accidental>(this->accidental_combobox->currentIndex());
    note.octDot = static_cast<Note::OctDot>(this->octdot_combobox->currentIndex());
    note.rect = QRect(); // 设置矩形为空
    this->result = note; // 保存结果
    this->accept();      // 接受对话框（返回QDialog::Accepted）
}

// onCancel槽：处理取消按钮点击事件
void Modification::onCancel()
{
    this->reject(); // 拒绝对话框（返回QDialog::Rejected）
}

// getResult方法：获取修改后的音符结果
Note Modification::getResult() const
{
    return this->result; // 返回结果
}