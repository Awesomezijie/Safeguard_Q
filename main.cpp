// ============================================================
//                        Safeguard_Q 
// ============================================================
//架构师（负责人）：卡卡



//你懂的
#define _CRT_SECURE_NO_WARNINGS



//有不认识的《去头文件说明》文件看
#include <graphics.h>
#include <conio.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <fstream>
#include <windows.h>



// ------------------------------------------------------------
// 窗口与帧率
// ------------------------------------------------------------

//宏常量处理
// 知识点补充：翁恺 MOOC 《C 语言程序设计进阶》 第 12 周：编译预处理和宏（其实不看也可以）
#define WIN_W 1280                 // 窗口宽
#define WIN_H 720                  // 窗口高
#define UI_H 80                    // 底部UI栏高
#define TOP_Y 0                    // 游戏区顶部Y
#define BOT_Y (WIN_H - UI_H)       // 游戏区底部Y（UI栏顶）
#define FPS 20                     // 帧率
#define FRAME_MS (1000 / FPS)      // 每帧毫秒数




// ------------------------------------------------------------
// 地图坐标
// ------------------------------------------------------------
#define PATH_Y (BOT_Y / 2)        // 敌人路径Y坐标（游戏区垂直居中，即640/2=320）
#define BASE_X (WIN_W / 10)        // 大本营X（128）
#define BASE_Y PATH_Y              // 大本营Y（320）

// 敌人生成点：屏幕右侧外
#define SPAWN_X (WIN_W + 50)       // 1330

// 矿区：大本营左上方 / 左下方
#define MINE1_X (BASE_X / 3)       // 上矿区X（42）
#define MINE1_Y (PATH_Y - 120)     // 上矿区Y（200）
#define MINE2_X (BASE_X / 3)       // 下矿区X（42）
#define MINE2_Y (PATH_Y + 120)     // 下矿区Y（440）

// 防御单位站位：盾卫紧贴大本营右侧，弓箭手在盾卫右侧
#define SHIELD_X (BASE_X + 60)     // 盾卫X（188）
#define SHIELD_Y PATH_Y            // 盾卫Y（320）
#define ARCHER_X (BASE_X + 140)    // 弓箭手X（268，盾卫右侧）
#define ARCHER_Y PATH_Y            // 弓箭手Y（320）

// ------------------------------------------------------------
// 游戏状态枚举
// ------------------------------------------------------------
#define STATE_EXIT        (-1)     // 退出游戏状态
#define STATE_MENU         0       // 主菜单
#define STATE_GAME         1       // 游戏主界面
#define STATE_VICTORY      2       // 胜利结算
#define STATE_DEFEAT       3       // 失败结算
#define STATE_PAUSE        4       // 暂停界面
#define STATE_SETTING      5       // 设置界面（UI-06）
#define STATE_SAVE_MANAGE  6       // 存档管理（UI-07）
#define STATE_LEVEL_SELECT 7       // 关卡选择（UI-08）
#define STATE_TEAM_INTRO   8       // 团队介绍（UI-09）
#define STATE_GAME_SETTING 9       // 游戏内设置（UI-10）

// ------------------------------------------------------------
// 单位类型枚举
// ------------------------------------------------------------

//enum 是 C 语言枚举类型（enumeration）。
//作用：把一组有含义的整数常量打包在一起，起名字，提高代码可读性。

typedef enum {
    UT_MINER = 0,      // 矿工：产金币
    UT_SHIELD,         // 盾卫：购买时大本营最大血量+10
    UT_ARCHER,         // 弓箭手：远程射击
    UT_WARRIOR,        // 将士：近战攻击
    UT_GENERAL,        // 将军：强化版将士（5倍血量/攻击，0.7倍速度）
    UT_TYPE_COUNT      // 代表前面枚举元素总个数，，，如同len、size一样用于数组。。
} UnitType;

// ------------------------------------------------------------
// 敌人类型枚举
// ------------------------------------------------------------
typedef enum {
    ET_VARIABLE = 0,   // “变量”敌人（血量20 攻击5 速度1）
    ET_ARRAY,          // “数组”敌人（血量40 攻击10 速度2）
    ET_POINTER,        // “指针”敌人（血量60 攻击15 速度1）
    ET_TYPE_COUNT
} EnemyType;

// ------------------------------------------------------------
// 数据结构定义
// ------------------------------------------------------------

// 我方单位
typedef struct Unit {
    int type;            // 单位类型（UnitType）
    int x, y;            // 坐标
    int hp, maxHp;       // 当前血量 / 最大血量
    int atk;             // 攻击力
    int speed;           // 移动速度（像素/帧）
    int cd, cdMax;       // 攻击/生产冷却计时 / 冷却上限（帧）
    bool alive;          // 是否存活
} Unit;

// 敌人
typedef struct Enemy {
    int type;            // 敌人类型（EnemyType）
    int x, y;            // 坐标
    int dx;              // 移动方向（-1向左，+1向右）
    int hp, maxHp;       // 当前血量 / 最大血量
    int atk;             // 攻击力
    int speed;           // 移动速度
    int cd, cdMax;       // 攻击冷却计时 / 上限（帧）
    bool alive;          // 是否存活
} Enemy;

// 箭矢
typedef struct Arrow {
    int x, y;            // 坐标
    int dx;              // 水平移动方向（+1向右）
    bool alive;          // 是否有效
} Arrow;

// 存档数据
typedef struct SaveData {
    int gold;                    // 金币
    int baseHp;                  // 大本营当前血量
    int baseMaxHp;               // 大本营最大血量
    int totalSpawned;            // 已生成敌人总数
    int killedCount;             // 消灭敌人数
    int bombCooldown;            // 核弹冷却剩余帧数
    bool bombAvailable;          // 核弹是否可用（答题失败后禁用）
    int gameTime;                // 游戏帧计数
    int currentLevel;            // 当前关卡（1,2,...）
    int unitCount[UT_TYPE_COUNT]; // 各职业当前数量
    Unit myUnits[50];            // 我方单位数组
    Enemy enemies[30];           // 敌人数组
    Arrow arrows[50];            // 箭矢数组
    int myUnitCount;             // 我方单位总数
    int enemyCount;              // 敌人总数
    int arrowCount;              // 箭矢总数
    time_t saveTime;             // 存档时间戳（显示时用localtime转换）
} SaveData;

// 关卡配置
typedef struct LevelConfig {
    int totalEnemies;      // 本关敌人数
    int spawnInterval;     // 敌人生成间隔（帧）
    float hpMul;           // 敌人血量倍率
    float atkMul;          // 敌人攻击力倍率
    int weightVar;         // 变量敌人权重
    int weightArr;         // 数组敌人权重
    int weightPtr;         // 指针敌人权重
} LevelConfig;
//权重：控制随机生成敌人的时候，每种敌人出现的概率。权重数字越大，这个类型敌人被抽出来的机会就越高



// 团队信息
typedef struct TeamMember {
    TCHAR name[50];         // 修改为TCHAR
    TCHAR role[50];
    TCHAR position[50];
    TCHAR module[100];
} TeamMember;


// 核弹答题题目 —— 改为TCHAR，消除宽窄字符混用
typedef struct Question {
    TCHAR text[200];               //题干
    TCHAR options[4][50];          //四个选项
    int answer;                    //正确答案下标（0~3）
} Question;



// ------------------------------------------------------------
// 关键数值常量
// ------------------------------------------------------------
#define MY_UNIT_MAX 80           //我方最大人数
#define ENEMY_MAX   50           //敌方最大人数
#define SAVE_SLOT_COUNT 3        // 存档槽位数
#define MINER_PRODUCE_FRAME 20   // 矿工产金币间隔（帧，2秒）   改为1 秒产金币
#define ARCHER_CD_FRAME 10       // 弓箭手攻击间隔（帧，1.5秒） 改为0.5秒
#define ARROW_SPEED 10           // 箭矢移动速度（像素/帧），可自行调整
#define ARROW_MAX 50             // 箭矢最大数量（与箭矢数组大小一致）
#define ARROW_HIT_RADIUS 20      // 碰撞判定半径
#define ARROW_HIT_RADIUS_SQ (ARROW_HIT_RADIUS * ARROW_HIT_RADIUS)
#define BOMB_COOLDOWN_FRAME 400  // 核弹冷却（帧，20秒）
#define BOMB_KEY 'N'             // 核弹触发键（补充.md规定N键，初稿无需答题）
#define SOLDIER_ATK_RANGE 40     //将士 / 将军近战攻击半径 40 像素
#define WARRIOR_CD_FRAME 30      // 将士/将军攻击冷却（帧）
#define ENEMY_ATK_RANGE 15       //敌人能打到我方单位的最远距离
#define ENEMY_ATK_CD_MAX 20      //敌人最大攻击冷却帧数
#define ENEMY_ATK_RANGE_SQ (ENEMY_ATK_RANGE * ENEMY_ATK_RANGE)
//攻击距离的平方，用于不开根号做距离判断
#define BOMB_EFFECT_DURATION 30


// ------------------------------------------------------------
// 全局变量
// ------------------------------------------------------------

// 设置相关（UI-06 / UI-10共用）
bool soundOn = true;       // 音效开关（true开，false关）
int volume = 50;           // 音量（0~100）
bool musicOn = true;       // 音乐开关（true开，false关）
int difficulty = 1;        // 难度（0简单，1中等，2困难）


// 游戏核心状态
int state = STATE_MENU;    // 当前界面状态
int gold = 50;             // 金币数
int baseHp = 100;          // 大本营当前血量
int baseMaxHp = 100;       // 大本营最大血量
int totalSpawned = 0;      // 已生成敌人总数
int killedCount = 0;       // 已消灭敌人数
int bombCooldown = 0;      // 核弹冷却剩余帧数
bool bombAvailable = true; // 核弹是否可用（答错后禁用）
int gameTime = 0;          // 游戏帧计数
int gameSeconds = 0;       // 游戏秒数（gameTime每满FPS帧+1）
int currentLevel = 1;      // 当前关卡（从1开始）


// 各职业当前已购买并存活的数量
// 下标含义：0矿工 1盾卫 2弓箭手 3将士 4将军
// 购买成功后+1；对应单位死亡（alive置false）时-1
int unitCount[UT_TYPE_COUNT] = { 0, 0, 0, 0, 0 };


// 动态数组（定长）
Unit myUnits[MY_UNIT_MAX]; //我放单位数组
Enemy enemies[ENEMY_MAX];  //敌方单位数组
Arrow arrows[50];          // 箭矢数组
int myUnitCount = 0;       // 当前我方单位总数（含死亡标记）
int enemyCount = 0;        // 当前敌人总数（含死亡标记）
int arrowCount = 0;        // 当前箭矢总数（含死亡标记）


// 单位配置表（下标与UnitType对应）
int unitPrice[UT_TYPE_COUNT] = { 10, 20, 30, 50, 100 };   // 购买价格
int unitLimit[UT_TYPE_COUNT] = { 20, 10, 20, 20, 10 };    // 购买上限
int unitHp[UT_TYPE_COUNT] = { 50, 200, 30, 100, 500 };    // 基础血量（将军500=将士100×5）
int unitAtk[UT_TYPE_COUNT] = { 0, 0, 15, 10, 50 };        // 攻击力（将军50=将士10×5）
int unitSpeed[UT_TYPE_COUNT] = { 0, 0, 0, 2, 1 };         // 移动速度（将军1≈将士2×0.7）


// 敌人基础配置表（下标与EnemyType对应）
int enemyBaseHp[ET_TYPE_COUNT] = { 200, 700, 1000 };           // 基础血量
int enemyBaseAtk[ET_TYPE_COUNT] = { 5, 10, 20 };           // 基础攻击
int enemyBaseSpeed[ET_TYPE_COUNT] = { 3, 2, 1 };           // 移动速度


// 关卡配置表
LevelConfig levelConfigs[] = {
    { 50, 60, 1.0f, 1.0f, 5, 3, 2 },   // 关卡一：50敌，60帧生成一个
    { 30, 50, 1.5f, 1.3f, 3, 4, 3 }    // 关卡二（拓展）
};
int levelCount = sizeof(levelConfigs) / sizeof(levelConfigs[0]);



/*
// 团队信息数组
TeamMember team[] = {
    { "丹丹", "大本营", "组长", "全局状态、购买单位、胜负判定、数值" },
    { "嘉豪", "盾卫"  , "副组长", "纯 C 逻辑，敌人系统、核弹清屏" },
    { "卡卡", "将军", "技术官", "main () 主循环 + 状态机、弓箭手模块、全项目整合、最终审查发布" },
    { "Three", "将士", "产品经理", "简单逻辑、简单界面，协助其他人、测试" },
    { "小猴子", "弓箭手",   "技术官", "界面绘制、键盘鼠标交互" },
    { "寻光", "矿工", "监督官", "界面绘制、交互、将士将军、答题" },
    {"朱涛","数组","信息官","碰撞函数、全流程测试" },
    {"耿耿","核弹","小学长","吃瓜" }
    //友情客串：东东哥
};
int teamCount = sizeof(team) / sizeof(team[0]);



// 核弹题库
Question questions[] = {
    { "C语言中，用于定义整型变量的关键字是？",
      {"int", "float", "char", "double"}, 0 },
    { "数组的下标起始值是什么？",
      {"0", "1", "-1", "任意"}, 0 },
    { "指针变量存储的是什么？",
      {"地址", "值", "数组", "函数"}, 0 }
};
int questionCount = sizeof(questions) / sizeof(questions[0]);
*/



// 团队信息数组
TeamMember team[] = {
    { _T("丹丹"), _T("大本营"), _T("组长"), _T("全局状态、购买单位、胜负判定、数值") },
    { _T("嘉豪"), _T("盾卫"), _T("副组长"), _T("纯 C 逻辑，敌人系统、核弹清屏") },
    { _T("卡卡"), _T("将军"), _T("技术官"), _T("main () 主循环 + 状态机、弓箭手模块、全项目整合、最终审查发布") },
    { _T("Three"), _T("将士"), _T("产品经理"), _T("简单逻辑、简单界面，协助其他人、测试") },
    { _T("小猴子"), _T("弓箭手"), _T("技术官"), _T("界面绘制、键盘鼠标交互") },
    { _T("寻光"), _T("矿工"), _T("监督官"), _T("界面绘制、交互、将士将军、答题") },
    { _T("朱涛"), _T("数组"), _T("信息官"), _T("碰撞函数、全流程测试") },
    { _T("耿耿"), _T("核弹"), _T("小学长"), _T("吃瓜") }
};
int teamCount = sizeof(team) / sizeof(team[0]);



// 核弹题库
Question questions[] = {
    { _T("C语言中，用于定义整型变量的关键字是？"),
      {_T("int"), _T("float"), _T("char"), _T("double")}, 0 },
    { _T("数组的下标起始值是什么？"),
      {_T("0"), _T("1"), _T("-1"), _T("任意")}, 0 },
    { _T("指针变量存储的是什么？"),
      {_T("地址"), _T("值"), _T("数组"), _T("函数")}, 0 }
};
int questionCount = sizeof(questions) / sizeof(questions[0]);









//drawBackground
bool imagesLoaded = false;
IMAGE bg1,bg2,bg3; // 背景图片
IMAGE imgMiner, imgShield, imgArcher, imgWarrior, imgGeneral;
IMAGE imgEnemyVar, imgEnemyArray, imgEnemyPointer;
IMAGE imgBase, imgBomb, imgGold;
//useBomb()
bool isBombAnswering = false;     // 是否正在答核弹题
int currentBombQIdx = 0;          // 当前展示的题目下标 
bool bombEffectActive = false;    // 核弹特效是否激活
int bombEffectFrame = 0;          // 核弹特效当前帧
int bombEffectX = 0, bombEffectY = 0; // 特效中心点坐标




// ------------------------------------------------------------
// 界面层函数声明（view）
// ------------------------------------------------------------

/*
 * 负责人：待分配
 * 功能：主菜单界面
 *   1. 绘制标题、按钮（开始/读档/设置/团队/退出）
 *   2. 处理鼠标点击，切换state
 * 参数：无
 * 返回值：无
 */
void menuView();

/*
 * 负责人：待分配
 * 功能：游戏主界面（核心玩法循环）
 *   1. 绘制游戏画面
 *   2. 处理购买、核弹、暂停等输入
 *   3. 每帧调用逻辑更新函数
 * 参数：无
 * 返回值：无
 */
void gameView();

/*
 * 负责人：待分配
 * 功能：胜利结算界面
 * 参数：无
 * 返回值：无
 */
void victoryView();

/*
 * 负责人：待分配
 * 功能：失败结算界面
 * 参数：无
 * 返回值：无
 */
void defeatView();

/*
 * 负责人：待分配
 * 功能：暂停界面
 * 参数：无
 * 返回值：无
 */
void pauseView();

/*
 * 负责人：待分配
 * 功能：设置界面（UI-06，主菜单/暂停共用）
 * 参数：无
 * 返回值：无
 */
void settingView();

/*
 * 负责人：待分配
 * 功能：存档管理界面（UI-07）
 * 参数：无
 * 返回值：无
 */
void saveManageView();

/*
 * 负责人：待分配
 * 功能：关卡选择界面（UI-08）
 * 参数：无
 * 返回值：无
 */
void levelSelectView();

/*
 * 负责人：待分配
 * 功能：团队介绍界面（UI-09）
 * 参数：无
 * 返回值：无
 */
void teamIntroView();

/*
 * 负责人：待分配
 * 功能：游戏内设置界面（UI-10，游戏中调用，返回游戏）
 * 参数：无
 * 返回值：无
 */
void gameSettingView();

// ------------------------------------------------------------
// 逻辑层函数声明（service）
// ------------------------------------------------------------

/*
 * 负责人：待分配
 * 功能：初始化新游戏
 *   1. 重置全局变量（gold=50, baseHp=100等）
 *   2. 清空所有数组
 *   3. 根据currentLevel读取关卡配置
 *   4. state = STATE_GAME
 * 参数：无
 * 返回值：无
 */
void initGame();

/*
 * 负责人：待分配
 * 功能：购买单位
 *   1. 检查参数合法性、金币、上限
 *   2. 扣除金币，unitCount[type]++
 *   3. 生成单位到对应位置
 * 特殊：type==UT_SHIELD时，baseMaxHp += 10
 * 参数：type：单位类型（UnitType）
 * 返回值：true-成功；false-失败
 */
bool buyUnit(int type);

/*
 * 负责人：待分配
 * 功能：矿工产金币
 *   每MINER_PRODUCE_FRAME帧，存活的矿工产出1金币
 * 参数：无
 * 返回值：无
 */
void updateMiners();

/*
 * 负责人：待分配
 * 功能：敌人生成
 *   使用 gameTime % spawnInterval == 0 判断生成时机
 *   超过本关总敌数后不再生成
 * 参数：无
 * 返回值：无
 */
void spawnEnemy();

/*
 * 负责人：待分配
 * 功能：更新敌人行为
 *   移动、攻击单位、到达大本营扣血
 * 参数：无
 * 返回值：无
 */
void updateEnemies();

/*
 * 负责人：待分配
 * 功能：更新弓箭手行为
 *   冷却结束且场上有敌人时，向最左侧敌人发射箭矢
 * 参数：无
 * 返回值：无
 */
void updateArchers();

/*
 * 负责人：待分配
 * 功能：更新箭矢移动与碰撞
 * 参数：无
 * 返回值：无
 */
void updateArrows();

/*
 * 负责人：待分配
 * 功能：更新将士/将军行为
 * 参数：无
 * 返回值：无
 */
void updateSoldiers();

/*
 * 负责人：待分配
 * 功能：核弹答题（拓展）
 * 初稿直接使用，不用答题；后续版本启用
 * 参数：无
 * 返回值：true-答对；false-答错
 */
bool askQuestion();

/*
 * 负责人：待分配
 * 功能：释放核弹
 * 初稿：按N键，冷却结束直接clearEnemies()
 * 拓展：答题成功后清屏，答错bombAvailable=false
 * 参数：无
 * 返回值：无
 */
void useBomb();

/*
 * 负责人：待分配
 * 功能：清空场上所有存活敌人
 *   遍历enemies，统计alive数量并累加killedCount，然后置false
 * 参数：无
 * 返回值：无
 */
void clearEnemies();

/*
 * 负责人：待分配
 * 功能：胜负判定
 *   失败：baseHp <= 0
 *   胜利：满足三个条件（生成完、场上无存活敌人、baseHp>0）
 * 注意：场上无存活敌人必须遍历enemies检查alive，不能只比较enemyCount
 * 参数：无
 * 返回值：无
 */
void checkGameStatus();

// ------------------------------------------------------------
// 存档相关函数声明
// ------------------------------------------------------------

/*
 * 负责人：待分配
 * 功能：保存游戏到指定槽位
 * 参数：slot - 存档槽位（0~SAVE_SLOT_COUNT-1）
 * 返回值：true-成功；false-失败
 */
bool saveGame(int slot);

/*
 * 负责人：待分配
 * 功能：从指定槽位读取存档
 * 参数：slot - 存档槽位
 * 返回值：true-成功；false-失败
 */
bool loadGame(int slot);

/*
 * 负责人：待分配
 * 功能：检查槽位是否有存档
 * 参数：slot - 存档槽位
 * 返回值：true-有；false-无
 */
bool hasSave(int slot);

/*
 * 负责人：待分配
 * 功能：删除指定槽位存档
 * 参数：slot - 存档槽位
 * 返回值：无
 */
void deleteSave(int slot);

// ------------------------------------------------------------
// 设置相关函数声明
// ------------------------------------------------------------

/*
 * 负责人：待分配
 * 功能：修改设置
 * 参数：
 *   option：0音效 1音量 2难度 3音乐
 *   value：新值（音量0~100，难度/开关0/1/2）
 * 返回值：无
 */
void changeSetting(int option, int value);

// ------------------------------------------------------------
// 辅助函数声明
// ------------------------------------------------------------

/*
 * 负责人：待分配
 * 功能：绘制通用背景（路径、UI底栏）
 * 参数：无
 * 返回值：无
 */
void drawBackground();

/*
 * 负责人：待分配
 * 功能：寻找最左侧存活敌人
 * 参数：无
 * 返回值：敌人数组下标，无返回-1
 */
int findLeftmostEnemy();

/*
 * 负责人：待分配
 * 功能：判断点(x,y)附近是否有敌人
 * 参数：x,y坐标；range判定半径
 * 返回值：有true，无false
 */
bool isEnemyNear(int x, int y, int range);

/*
 * 负责人：待分配
 * 功能：清空所有数组（alive全false，计数归0）
 * 参数：无
 * 返回值：无
 */
void clearArrays();









// ============================================================
// 界面层实现（占位）
// ============================================================








//26.8.21小猴子
//=============================================================
/*
 * 负责人：小猴子
 * 功能：绘制游戏静态背景
 *   1. 草地底色与纹理
 *   2. 敌人行进土路
 *   3. 上下两个矿区
 *   注意：本函数只画背景，不负责清屏（由 gameView 开头 cleardevice）
 * 参数：无
 * 返回值：无
 * 涉及全局变量：WIN_W、WIN_H、PATH_Y/BASE_Y、MINE1_X/Y、MINE2_X/Y
 */


// ---------- 辅助函数（函数外部定义） ----------
bool loadAllImages() {
    // 检查文件是否存在
    // C++ lambda、std::ifstream(wunaizhiju)
    auto fileExists = [](const char* path)->bool {
        std::ifstream f(path);
        return f.good();
        };
    const char* resList[] = {
        "res/miner.png",
        "res/shield.png", 
        "res/archer.png",
        "res/warrior.png", 
        "res/general.png",
        "res/var.png",
        "res/array.png", 
        "res/pointer.png",
        "res/base.png", 
        "res/bomb.png", 
        "res/gold.png"
    };
    for (auto path : resList) {
        if (!fileExists(path)) {
            char msg[256];
            sprintf_s(msg, "缺失资源文件：%s\n请检查res文件夹！", path);
            MessageBox(NULL, msg, "图片加载失败", MB_ICONERROR);
            return false;
        }
    }
    // 加载图片
    loadimage(&imgMiner, "res/miner.png");
    loadimage(&imgShield, "res/shield.png");
    loadimage(&imgArcher, "res/archer.png");
    loadimage(&imgWarrior, "res/warrior.png");
    loadimage(&imgGeneral, "res/general.png");
    loadimage(&imgEnemyVar, "res/var.png");
    loadimage(&imgEnemyArray, "res/array.png");
    loadimage(&imgEnemyPointer, "res/pointer.png");
    loadimage(&imgBase, "res/base.png");
    loadimage(&imgBomb, "res/bomb.png");
    loadimage(&imgGold, "res/gold.png");
    loadimage(&bg1, "bg1.png", WIN_W, WIN_H);
    loadimage(&bg2, "bg2.png", WIN_W, WIN_H);
    loadimage(&bg3, "bg3.png", WIN_W, WIN_H);
    return true;
}
IMAGE* getUnitImg(int type) {
    switch (type) {
    case UT_MINER:   return &imgMiner;
    case UT_SHIELD:  return &imgShield;
    case UT_ARCHER:  return &imgArcher;
    case UT_WARRIOR: return &imgWarrior;
    case UT_GENERAL: return &imgGeneral;

    default:         return NULL;
    }
}
IMAGE* getEnemyImg(int type) {
    switch (type) {
    case ET_VARIABLE: return &imgEnemyVar;
    case ET_ARRAY:    return &imgEnemyArray;
    case ET_POINTER:  return &imgEnemyPointer;
    default:          return NULL;
    }
}



void drawBackground() {
    // TODO: 绘制背景、道路（PATH_Y）、UI底栏（BOT_Y区域）

    // ========== 1. 草地底色 ==========
    setfillcolor(RGB(34, 139, 34));
    solidrectangle(0, 55, WIN_W, WIN_H - 80);   // 顶部留55给HUD，底部留80给UI栏
    // 深绿色草地纹理（纵向色带）
    setfillcolor(RGB(28, 120, 28));
    for (int i = 0; i < WIN_W; i += 40) {
        solidrectangle(i, 55, i + 20, WIN_H - 80);
    }
    // 浅绿色草点（固定规律点缀）
    setfillcolor(RGB(100, 180, 100));
    for (int i = 0; i < WIN_W; i += 30) {
        for (int j = 60; j < WIN_H - 90; j += 30) {
            if ((i + j) % 60 == 0) {
                solidcircle(i, j, 3);
            }
        }
    }
    // ========== 2. 中间土路（敌人行进区域） ==========
    setfillcolor(RGB(139, 90, 43));
    solidrectangle(0, BASE_Y - 35, WIN_W, BASE_Y + 35);   // 道路宽度70像素
    // 土路边框
    setlinecolor(RGB(100, 60, 20));
    rectangle(0, BASE_Y - 35, WIN_W, BASE_Y + 35);
    // 路边小草装饰
    setfillcolor(RGB(20, 100, 20));
    for (int i = 0; i < WIN_W; i += 50) {
        solidrectangle(i, BASE_Y - 42, i + 8, BASE_Y - 38);   // 上方草
        solidrectangle(i, BASE_Y + 38, i + 8, BASE_Y + 42);   // 下方草
    }
    // ========== 3. 上方矿区 ==========
    setfillcolor(RGB(180, 180, 150));
    setlinecolor(RGB(100, 100, 80));
    solidrectangle(MINE1_X - 25, MINE1_Y - 25, MINE1_X + 25, MINE1_Y + 25);
    rectangle(MINE1_X - 25, MINE1_Y - 25, MINE1_X + 25, MINE1_Y + 25);
    setbkmode(TRANSPARENT);
    settextcolor(RGB(80, 80, 60));
    settextstyle(16, 0, _T("黑体"));
    outtextxy(MINE1_X - 12, MINE1_Y - 10, _T("矿区"));
    // ========== 4. 下方矿区 ==========
    setfillcolor(RGB(180, 180, 150));
    setlinecolor(RGB(100, 100, 80));
    solidrectangle(MINE2_X - 25, MINE2_Y - 25, MINE2_X + 25, MINE2_Y + 25);
    rectangle(MINE2_X - 25, MINE2_Y - 25, MINE2_X + 25, MINE2_Y + 25);
    setbkmode(TRANSPARENT);
    settextcolor(RGB(80, 80, 60));
    settextstyle(16, 0, _T("黑体"));
    outtextxy(MINE2_X - 12, MINE2_Y - 10, _T("矿区"));

    // ========== 还原基础绘图状态 ==========
    setfillcolor(WHITE);
    setlinecolor(BLACK);
    settextcolor(BLACK);
    settextstyle(20, 0, _T("宋体"));

}
//=============================================================










//26.8.21寻光
//=============================================================
// ------------------------------------------------------------
// 主菜单模块
// 负责人：寻光
// 说明：负责主菜单（UI-01）的界面绘制与鼠标交互。
//       本模块为单帧函数，由主循环调度，内部禁止 while(1)。
// ------------------------------------------------------------

// 主菜单按钮配置
#define MENU_BTN_COUNT 5                       // 按钮总数
#define MENU_BTN_WIDTH 240                     // 按钮宽度
#define MENU_BTN_HEIGHT 55                     // 按钮高度
#define MENU_BTN_GAP 18                        // 按钮间距

// 按钮坐标数组（由 initMenuButtons 计算）
static int menuBtnX[MENU_BTN_COUNT];
static int menuBtnY[MENU_BTN_COUNT];

// 按钮可用状态：1可点，0置灰
// 下标含义：0开始新游戏 1读取存档 2游戏设置 3团队介绍 4退出游戏
static int menuBtnEnabled[MENU_BTN_COUNT] = { 1, 0, 0, 1, 1 };

// 当前选中按钮下标（默认选中"开始新游戏"，配合键盘导航预留）
static int selectedIndex = 0;

/*
 * 负责人：寻光
 * 功能：初始化主菜单按钮的位置
 *   根据 WIN_W / WIN_H 居中计算各按钮的左上角坐标
 * 参数：无
 * 返回值：无
 * 涉及全局变量：menuBtnX、menuBtnY
 */
static void initMenuButtons() {
    int startX = (WIN_W - MENU_BTN_WIDTH) / 2;          // 水平居中
    int totalH = MENU_BTN_HEIGHT * MENU_BTN_COUNT
        + MENU_BTN_GAP * (MENU_BTN_COUNT - 1);          // 全部按钮总高度
    int startY = (WIN_H - totalH) / 2;                  // 垂直居中

    for (int i = 0; i < MENU_BTN_COUNT; i++) {
        menuBtnX[i] = startX;
        menuBtnY[i] = startY + i * (MENU_BTN_HEIGHT + MENU_BTN_GAP);
    }
}

/*
 * 负责人：寻光
 * 功能：绘制单个主菜单按钮
 *   1. 置灰按钮：灰色底 + 灰色文字
 *   2. 选中按钮：浅蓝底 + 白色粗边框
 *   3. 普通按钮：蓝底黑字
 * 参数：index - 按钮下标；text - 按钮文字
 * 返回值：无
 * 涉及全局变量：menuBtnX、menuBtnY、menuBtnEnabled、selectedIndex
 */
static void drawMenuButton(int index, const char* text) {
    int left = menuBtnX[index];
    int top = menuBtnY[index];
    int right = left + MENU_BTN_WIDTH;
    int bottom = top + MENU_BTN_HEIGHT;

    if (!menuBtnEnabled[index]) {                       // 置灰
        setfillcolor(RGB(150, 150, 150));
        setlinecolor(RGB(90, 90, 90));
        setlinestyle(PS_SOLID, 1);
    }
    else if (index == selectedIndex) {                  // 选中高亮
        setfillcolor(RGB(100, 160, 255));
        setlinecolor(RGB(255, 255, 255));
        setlinestyle(PS_SOLID, 3);
    }
    else {                                              // 普通可用
        setfillcolor(RGB(70, 130, 240));
        setlinecolor(RGB(0, 0, 0));
        setlinestyle(PS_SOLID, 1);
    }

    solidrectangle(left, top, right, bottom);           // 按钮底色
    rectangle(left, top, right, bottom);                // 按钮边框
    setlinestyle(PS_SOLID, 1);                          // 恢复线宽

    // 按钮文字（水平垂直居中）
    setbkmode(TRANSPARENT);                             // 文字背景透明
    settextstyle(20, 0, "宋体");
    settextcolor(menuBtnEnabled[index] ? BLACK : RGB(80, 80, 80));
    int tw = textwidth(text);
    int th = textheight(text);
    outtextxy(left + (MENU_BTN_WIDTH - tw) / 2, top + (MENU_BTN_HEIGHT - th) / 2, text);
}

/*
 * 负责人：寻光
 * 功能：主菜单界面（UI-01）
 *   1. 绘制背景、标题、5个按钮、小组标识
 *   2. 鼠标悬停切换选中
 *   3. 鼠标左键点击按钮：置灰按钮弹提示；可用按钮切换 state
 * 参数：无
 * 返回值：无
 * 涉及全局变量：state、selectedIndex、menuBtnX、menuBtnY、menuBtnEnabled
 * 注意：单帧函数，由主循环调度，内部禁止 while(1)
 */
void menuView() {
    cleardevice();

    // ========== 加载并绘制主菜单背景 ==========
    static IMAGE bg1;          // 静态图片变量，只加载一次
    static bool bgLoaded = false;
    if (!bgLoaded) {
        loadimage(&bg1, _T("res/bg1.png"), WIN_W, WIN_H);  // 缩放到窗口大小
        bgLoaded = true;
    }
    putimage(0, 0, &bg1);      // 从窗口左上角开始绘制，铺满整个窗口

    //drawBackground();               // 绘制通用背景（他人负责）
    // TODO: 绘制标题、按钮、处理鼠标点击
    initMenuButtons();              // 计算按钮坐标

    // 右上角游戏标题
    settextstyle(28, 0, "宋体");
    settextcolor(WHITE);
    setbkmode(TRANSPARENT);
    outtextxy(WIN_W - textwidth("Safeguard_Q（保护皮丹）") - 30, 25, "Safeguard_Q（保护皮丹）");

    // 中间五个按钮（顺序与菜单布局一致）
    const char* labels[MENU_BTN_COUNT] = {
        "开始新游戏", "读取存档", "游戏设置", "团队介绍", "退出游戏"
    };
    for (int i = 0; i < MENU_BTN_COUNT; i++) {
        drawMenuButton(i, labels[i]);
    }

    // 左下角小组标识（UI-01 需求）
    settextstyle(16, 0, "宋体");
    settextcolor(RGB(150, 150, 150));
    outtextxy(10, WIN_H - 30, "GrapeStudio");

    // 鼠标交互（peekmessage，架构统一风格）
    ExMessage msg;
    while (peekmessage(&msg, EM_MOUSE)) {
        if (msg.message == WM_MOUSEMOVE) {
            // 悬停切换选中（置灰按钮不参与选中）
            for (int i = 0; i < MENU_BTN_COUNT; i++) {
                if (menuBtnEnabled[i] &&
                    msg.x >= menuBtnX[i] && msg.x <= menuBtnX[i] + MENU_BTN_WIDTH &&
                    msg.y >= menuBtnY[i] && msg.y <= menuBtnY[i] + MENU_BTN_HEIGHT) {
                    selectedIndex = i;
                    break;
                }
            }
        }
        else if (msg.message == WM_LBUTTONDOWN) {
            // 先判断点到哪个按钮（含置灰按钮）
            int hit = -1;
            for (int i = 0; i < MENU_BTN_COUNT; i++) {
                if (msg.x >= menuBtnX[i] && msg.x <= menuBtnX[i] + MENU_BTN_WIDTH &&
                    msg.y >= menuBtnY[i] && msg.y <= menuBtnY[i] + MENU_BTN_HEIGHT) {
                    hit = i;
                    break;
                }
            }
            if (hit < 0) continue;   // 没点到按钮，忽略

            // 置灰按钮：必须给玩家反馈
            if (!menuBtnEnabled[hit]) {
                MessageBox(GetHWnd(), "该功能暂未开放，敬请期待！", "提示", MB_OK);
                continue;
            }

            // 可用按钮：执行对应逻辑并切换状态
            selectedIndex = hit;
            switch (hit) {
            case 0:                         // 开始新游戏
                initGame();                 // 初始化游戏数据（丹丹负责）
                state = STATE_GAME;
                break;
            case 3:                         // 团队介绍
                state = STATE_TEAM_INTRO;
                break;
            case 4:                         // 退出游戏
                state = STATE_EXIT;         // 通知主循环退出
                break;
            }
            break;                          // 已切换状态，跳出消息循环
        }
    }
}

//=============================================================







//8.21~22小猴子
//=============================================================
/*
 * 负责人：小猴子
 * 功能：游戏主界面（核心玩法循环）
 *   1. 加载图片（首次进入时加载）
 *   2. 绘制背景、大本营、矿区、土路
 *   3. 处理玩家输入：购买、核弹（按N）、暂停（Esc）、返回主菜单（Q）
 *   4. 更新游戏逻辑（答题时暂停更新）
 *   5. 绘制所有动态元素、HUD、底部UI栏、核弹特效
 *   6. 答题弹窗放在最后绘制，确保在最上层，弹窗输入由 askQuestion 统一处理
 * 参数：无
 * 返回值：无
 */

void gameView() {
    cleardevice();

    // ========== 第一步：加载图片（只执行一次） ==========
    if (!imagesLoaded) {
        if (!loadAllImages()) {
            imagesLoaded = true;
            return;
        }
        imagesLoaded = true;
    }

    // ========== 第二步：绘制背景 ==========
    drawBackground();

    // TODO: 绘制游戏画面、处理输入
    // 逻辑更新顺序（务必保持）：
    //   gameTime++;
    //   if (gameTime % FPS == 0) gameSeconds++;
    //   updateMiners();
    //   spawnEnemy();
    //   updateEnemies();
    //   updateArchers();
    //   updateArrows();
    //   updateSoldiers();
    //   checkGameStatus();

    // ---- 绘制大本营 ----
    
    putimage(BASE_X - 32, BASE_Y - 32, &imgBase);
    
    setbkmode(TRANSPARENT);
    settextcolor(RGB(80, 80, 80));
    settextstyle(14, 0, _T("黑体"));
    outtextxy(BASE_X - 20, BASE_Y - 65, _T("大本营"));

    // 大本营血条
    int hpBarMaxWidth = 80;
    int hpBarWidth = (baseHp * hpBarMaxWidth) / baseMaxHp;
    if (hpBarWidth < 0) hpBarWidth = 0;
    setfillcolor(RGB(200, 200, 200));
    solidrectangle(BASE_X - 40, BASE_Y - 48, BASE_X + 40, BASE_Y - 36);
    if (baseHp > baseMaxHp * 0.5) {
        setfillcolor(RGB(0, 200, 0));
    }
    else if (baseHp > baseMaxHp * 0.25) {
        setfillcolor(RGB(255, 200, 0));
    }
    else {
        setfillcolor(RGB(255, 50, 50));
    }
    solidrectangle(BASE_X - 40, BASE_Y - 48, BASE_X - 40 + hpBarWidth, BASE_Y - 36);
    setlinecolor(RGB(100, 100, 100));
    rectangle(BASE_X - 40, BASE_Y - 48, BASE_X + 40, BASE_Y - 36);
    TCHAR hpText[20];
    _stprintf_s(hpText, _countof(hpText), _T("%d/%d"), baseHp, baseMaxHp);
    settextcolor(BLACK);
    settextstyle(12, 0, _T("黑体"));
    outtextxy(BASE_X - 20, BASE_Y - 46, hpText);

    // ========== 第三步：处理输入（仅非答题状态） ==========
    if (!isBombAnswering) {
        ExMessage msg;
        // 键盘
        while (peekmessage(&msg, EX_KEY)) {
            if (msg.message == WM_KEYDOWN) {
                switch (msg.vkcode) {
                case '1': buyUnit(UT_MINER); break;
                case '2': buyUnit(UT_SHIELD); break;
                case '3': buyUnit(UT_ARCHER); break;
                case '4': buyUnit(UT_WARRIOR); break;
                case '5': buyUnit(UT_GENERAL); break;
                case 'N':
                case 'n': useBomb(); break;
                case 'Q':
                case 'q': state = STATE_MENU; break;
                case VK_ESCAPE: state = STATE_PAUSE; break;
                }
            }
        }
        // 鼠标购买
        while (peekmessage(&msg, EX_MOUSE)) {
            if (msg.message == WM_LBUTTONDOWN) {
                int btnX[5] = { 30, 180, 330, 480, 630 };
                int btnY = BOT_Y + 8;
                int btnW = 120, btnH = 68;
                for (int i = 0; i < 5; i++) {
                    if (msg.x >= btnX[i] && msg.x <= btnX[i] + btnW &&
                        msg.y >= btnY && msg.y <= btnY + btnH) {
                        buyUnit(i);
                        break;
                    }
                }
            }
        }
    }
    // 答题状态下不处理输入，由 askQuestion 在最后统一处理

    // ========== 第四步：更新游戏逻辑（答题时暂停） ==========
    if (!isBombAnswering) {
        gameTime++;
        if (gameTime % FPS == 0) gameSeconds++;
        if (bombCooldown > 0) bombCooldown--;
        updateMiners();
        spawnEnemy();
        updateEnemies();
        updateArchers();
        updateArrows();
        updateSoldiers();
        checkGameStatus();
    }

    // ========== 第五步：绘制动态元素 ==========
    // 箭矢
    for (int i = 0; i < arrowCount; i++) {
        if (!arrows[i].alive) continue;
        setfillcolor(RGB(255, 255, 100));
        solidcircle(arrows[i].x, arrows[i].y, 6);
        setfillcolor(RGB(255, 255, 180));
        solidcircle(arrows[i].x - 8, arrows[i].y, 4);
        solidcircle(arrows[i].x - 15, arrows[i].y, 2);
    }

    // 敌人
    const TCHAR* enemyNames[] = { _T("变量"), _T("数组"), _T("指针") };
    for (int i = 0; i < enemyCount; i++) {
        if (!enemies[i].alive) continue;
        IMAGE* img = getEnemyImg(enemies[i].type);
        if (img != NULL) {
            putimage(enemies[i].x - 16, enemies[i].y - 16, img);
        }
        setbkmode(TRANSPARENT);
        settextcolor(BLACK);
        settextstyle(12, 0, _T("黑体"));
        outtextxy(enemies[i].x - 10, enemies[i].y - 40, enemyNames[enemies[i].type]);
        int eHpBarWidth = (enemies[i].hp * 30) / enemies[i].maxHp;
        setfillcolor(RGB(200, 200, 200));
        solidrectangle(enemies[i].x - 15, enemies[i].y - 32, enemies[i].x + 15, enemies[i].y - 26);
        setfillcolor(RGB(255, 50, 50));
        solidrectangle(enemies[i].x - 15, enemies[i].y - 32, enemies[i].x - 15 + eHpBarWidth, enemies[i].y - 26);
        setlinecolor(RGB(100, 100, 100));
        rectangle(enemies[i].x - 15, enemies[i].y - 32, enemies[i].x + 15, enemies[i].y - 26);
    }

    // 我方单位
    const TCHAR* unitNames[] = { _T("矿工"), _T("盾卫"), _T("弓箭手"), _T("将士"), _T("将军") };
    for (int i = 0; i < myUnitCount; i++) {
        if (!myUnits[i].alive) continue;
        IMAGE* img = getUnitImg(myUnits[i].type);
        if (img != NULL) {
            putimage(myUnits[i].x - 16, myUnits[i].y - 16, img);
        }
        setbkmode(TRANSPARENT);
        settextcolor(BLACK);
        settextstyle(12, 0, _T("黑体"));
        outtextxy(myUnits[i].x - 12, myUnits[i].y - 34, unitNames[myUnits[i].type]);
        int uHpBarWidth = (myUnits[i].hp * 30) / myUnits[i].maxHp;
        setfillcolor(RGB(200, 200, 200));
        solidrectangle(myUnits[i].x - 15, myUnits[i].y - 26, myUnits[i].x + 15, myUnits[i].y - 20);
        setfillcolor(RGB(0, 200, 0));
        solidrectangle(myUnits[i].x - 15, myUnits[i].y - 26, myUnits[i].x - 15 + uHpBarWidth, myUnits[i].y - 20);
        setlinecolor(RGB(100, 100, 100));
        rectangle(myUnits[i].x - 15, myUnits[i].y - 26, myUnits[i].x + 15, myUnits[i].y - 20);
    }

    // ========== 第六步：绘制顶部HUD ==========
    setfillcolor(RGB(30, 30, 35));
    solidrectangle(0, 0, WIN_W, 55);
    setlinecolor(RGB(80, 80, 90));
    line(0, 55, WIN_W, 55);
    int sectionW = WIN_W / 4;

    // 分区1：金币
    setfillcolor(RGB(115, 208, 245));
    solidrectangle(0, 0, sectionW, 55);
    putimage(10, 12, &imgGold);
    setbkmode(TRANSPARENT);
    settextcolor(RGB(255, 215, 0));
    settextstyle(24, 0, _T("黑体"));
    TCHAR goldText[20];
    _stprintf_s(goldText, _countof(goldText), _T("x %d"), gold);
    outtextxy(48, 14, goldText);
    line(sectionW, 5, sectionW, 50);

    // 分区2：大本营血条
    setfillcolor(RGB(115, 208, 245));
    solidrectangle(sectionW, 0, sectionW * 2, 55);
    int hpBarX = sectionW + 85;
    int hpBarY = 18;
    int hpBarW = 110;
    int hpBarH = 18;
    if (hpBarX + hpBarW > sectionW * 2 - 10) hpBarW = sectionW * 2 - 10 - hpBarX;
    setfillcolor(RGB(50, 50, 50));
    solidrectangle(hpBarX, hpBarY, hpBarX + hpBarW, hpBarY + hpBarH);
    int hpWidth = (baseHp * hpBarW) / baseMaxHp;
    if (hpWidth < 0) hpWidth = 0;
    if (hpWidth > hpBarW) hpWidth = hpBarW;
    if (baseHp > baseMaxHp * 0.5) setfillcolor(RGB(0, 220, 0));
    else if (baseHp > baseMaxHp * 0.25) setfillcolor(RGB(255, 200, 0));
    else setfillcolor(RGB(255, 50, 50));
    solidrectangle(hpBarX, hpBarY, hpBarX + hpWidth, hpBarY + hpBarH);
    setlinecolor(RGB(120, 120, 120));
    rectangle(hpBarX, hpBarY, hpBarX + hpBarW, hpBarY + hpBarH);
    settextcolor(WHITE);
    settextstyle(16, 0, _T("SimHei"));
    outtextxy(sectionW + 10, 15, _T("大本营"));
    settextstyle(12, 0, _T("SimHei"));
    TCHAR tophpText[32];
    _stprintf_s(tophpText, _countof(tophpText), _T("%d / %d"), baseHp, baseMaxHp);
    int txtW = textwidth(tophpText);
    outtextxy(hpBarX + (hpBarW - txtW) / 2, hpBarY + 1, tophpText);
    line(sectionW * 2, 5, sectionW * 2, 50);

    // 分区3：敌人进度
    setfillcolor(RGB(115, 208, 245));
    solidrectangle(sectionW * 2, 0, sectionW * 3, 55);
    setfillcolor(RGB(255, 80, 80));
    fillcircle(sectionW * 2 + 20, 27, 10);
    int idx = currentLevel - 1;
    if (idx < 0 || idx >= levelCount) idx = 0;
    settextcolor(RGB(255, 200, 150));
    settextstyle(20, 0, _T("黑体"));
    TCHAR enemyText[30];
    _stprintf_s(enemyText, _countof(enemyText), _T("敌人 %d/%d"), totalSpawned, levelConfigs[idx].totalEnemies);
    outtextxy(sectionW * 2 + 40, 15, enemyText);
    line(sectionW * 3, 5, sectionW * 3, 50);

    // 分区4：核弹状态
    setfillcolor(RGB(115, 208, 245));
    solidrectangle(sectionW * 3, 0, WIN_W, 55);
    settextstyle(20, 0, _T("黑体"));
    if (bombCooldown > 0) {
        settextcolor(RGB(255, 200, 50));
        TCHAR bombText[30];
        _stprintf_s(bombText, _countof(bombText), _T("核弹 %ds"), bombCooldown / FPS);
        outtextxy(sectionW * 3 + 20, 15, bombText);
    }
    else if (bombAvailable) {
        settextcolor(RGB(50, 255, 50));
        outtextxy(sectionW * 3 + 20, 15, _T("核弹 就绪"));
    }
    else {
        settextcolor(RGB(150, 150, 150));
        outtextxy(sectionW * 3 + 20, 15, _T("核弹 禁用"));
    }
    settextcolor(RGB(150, 150, 150));
    settextstyle(14, 0, _T("黑体"));
    outtextxy(sectionW * 3 + 20, 38, _T("[N]"));

    // ========== 第七步：绘制底部UI栏 ==========
    setfillcolor(RGB(40, 40, 45));
    solidrectangle(0, BOT_Y, WIN_W, WIN_H);
    setlinecolor(RGB(80, 80, 90));
    line(0, BOT_Y, WIN_W, BOT_Y);
    const TCHAR* btnUnitNames[] = { _T("矿工"), _T("盾卫"), _T("弓箭手"), _T("将士"), _T("将军") };
    const int btnPrices[] = { 10, 20, 30, 50, 100 };
    const int btnX[] = { 30, 180, 330, 480, 630 };
    const int btnY = BOT_Y + 8;
    const int btnW = 120;
    const int btnH = 68;
    IMAGE* unitImages[] = { &imgMiner, &imgShield, &imgArcher, &imgWarrior, &imgGeneral };
    for (int i = 0; i < 5; i++) {
        int x = btnX[i];
        int y = btnY;
        bool canBuy = (gold >= btnPrices[i] && unitCount[i] < unitLimit[i]);
        if (canBuy) setfillcolor(RGB(50, 80, 120));
        else setfillcolor(RGB(60, 60, 60));
        solidroundrect(x, y, x + btnW, y + btnH, 8, 8);
        setlinecolor(RGB(180, 180, 180));
        roundrect(x, y, x + btnW, y + btnH, 8, 8);
        if (canBuy) {
            setlinecolor(RGB(255, 215, 0));
            setlinestyle(PS_SOLID, 2);
            roundrect(x + 2, y + 2, x + btnW - 2, y + btnH - 2, 6, 6);
            setlinestyle(PS_SOLID, 1);
        }
        if (unitImages[i] != NULL) {
            putimage(x + (btnW - 40) / 2, y + 4, unitImages[i]);
        }
        setbkmode(TRANSPARENT);
        settextcolor(RGB(200, 200, 200));
        settextstyle(12, 0, _T("黑体"));
        outtextxy(x + (btnW - 24) / 2, y + 46, btnUnitNames[i]);
        TCHAR priceText[20];
        _stprintf_s(priceText, _countof(priceText), _T("%d$"), btnPrices[i]);
        settextcolor(RGB(255, 215, 0));
        outtextxy(x + btnW - 35, y + btnH - 18, priceText);
        TCHAR countText[20];
        _stprintf_s(countText, _countof(countText), _T("%d/%d"), unitCount[i], unitLimit[i]);
        settextcolor(RGB(150, 150, 150));
        settextstyle(11, 0, _T("黑体"));
        outtextxy(x + 4, y + btnH - 18, countText);
        TCHAR keyText[10];
        _stprintf_s(keyText, _countof(keyText), _T("[%d]"), i + 1);
        settextcolor(RGB(255, 255, 200));
        settextstyle(11, 0, _T("黑体"));
        outtextxy(x + 4, y + 4, keyText);
    }

    // ========== 第八步：核弹爆炸特效 ==========
    if (bombEffectActive) {
        bombEffectFrame++;
        if (bombEffectFrame > BOMB_EFFECT_DURATION) {
            bombEffectActive = false;
        }
        else {
            float scale;
            if (bombEffectFrame < 10) scale = bombEffectFrame / 10.0f;
            else if (bombEffectFrame > BOMB_EFFECT_DURATION - 10) scale = (BOMB_EFFECT_DURATION - bombEffectFrame) / 10.0f;
            else scale = 1.0f;
            int size = (int)(48 * scale);
            int x = bombEffectX - size / 2;
            int y = bombEffectY - size / 2;
            putimage(x, y, size, size, &imgBomb, 0, 0);
            if (bombEffectFrame < 5) {
                setfillcolor(RGB(255, 255, 220));
                solidrectangle(0, 0, WIN_W, WIN_H);
            }

            putimage(bombEffectX - 120, bombEffectY - 120, &imgBomb);
        }
    }

    // ========== 第九步：答题弹窗（最后绘制，确保在最上层） ==========
    if (isBombAnswering) {
        askQuestion();
    }

    // ========== 第十步：还原绘图状态并刷新 ==========
    setfillcolor(WHITE);
    setlinecolor(BLACK);
    settextcolor(BLACK);
    settextstyle(20, 0, _T("宋体"));
    setbkmode(OPAQUE);

    FlushBatchDraw();

}
//=============================================================================================











//丹丹8.24
//==============================================================================================
//负责人：丹丹
// ------------------------------------------------------------
// victoryView() —— 胜利结算界面
// 返回类型：void
// 功能：
//   1. 在游戏画面上方叠加半透明遮罩
//   2. 显示 "Victory" 标题
//   3. 显示本局统计：剩余大本营血量 / 消灭敌人数 / 游戏时长
//   4. 显示“返回主菜单”按钮，点击后清空数据并回到主菜单
// 注意：
//   本函数被主循环每帧调用一次，不要写 while(1) 死循环。
//   为了“叠加”效果，这里不清屏（不调用 cleardevice），
//   直接在上一帧游戏画面上盖半透明遮罩。
//   如果你们组整合后发现屏幕是黑的，可以改成先绘图背景再遮罩。
//   消费本帧全部鼠标消息，使用标记变量，不阻塞消息队列。
// ------------------------------------------------------------
void victoryView() {
    //cleardevice();
    //drawBackground();
    // TODO: 显示胜利信息和统计

     // ---- 保存当前绘图状态，结束前完整恢复 ----
    COLORREF oldFill = getfillcolor();
    COLORREF oldLine = getlinecolor();
    COLORREF oldText = gettextcolor();
    int oldBkMode = getbkmode();
    LOGFONT oldFont;
    getfont(&oldFont);

    setbkmode(TRANSPARENT);   // 本函数内需要透明背景

    // 1. 半透明遮罩（ARGB，需双缓冲支持 DOUBLEBUF）
    setfillcolor(0x88000000);
    fillrectangle(0, 0, WIN_W, WIN_H);

    // 2. 标题 Victory（金色，居中）
    settextstyle(70, 0, _T("黑体"));
    settextcolor(RGB(255, 215, 0));
    int titleW = textwidth(_T("Victory"));
    outtextxy((WIN_W - titleW) / 2, 130, _T("Victory"));

    // 3. 统计信息（使用 TCHAR + _stprintf_s 兼容宽字符）
    settextstyle(26, 0, _T("宋体"));
    settextcolor(WHITE);
    TCHAR info[128];

    // 剩余大本营血量
    _stprintf_s(info, _countof(info), _T("剩余大本营血量：%d"), baseHp);
    int infoX = (WIN_W - textwidth(info)) / 2;
    outtextxy(infoX, 260, info);

    // 消灭敌人数
    _stprintf_s(info, _countof(info), _T("消灭敌人数：%d"), killedCount);
    infoX = (WIN_W - textwidth(info)) / 2;
    outtextxy(infoX, 300, info);

    // 游戏时长（分:秒）
    int minutes = gameSeconds / 60;
    int seconds = gameSeconds % 60;
    _stprintf_s(info, _countof(info), _T("游戏时长：%02d:%02d"), minutes, seconds);
    infoX = (WIN_W - textwidth(info)) / 2;
    outtextxy(infoX, 340, info);

    // 4. 返回主菜单按钮
    int btnW = 240;
    int btnH = 50;
    RECT btn;
    btn.left = (WIN_W - btnW) / 2;
    btn.top = 440;
    btn.right = btn.left + btnW;
    btn.bottom = btn.top + btnH;

    setfillcolor(RGB(60, 90, 130));
    setlinecolor(RGB(180, 200, 220));
    fillrectangle(btn.left, btn.top, btn.right, btn.bottom);

    settextstyle(24, 0, _T("宋体"));
    settextcolor(WHITE);
    int btnTextW = textwidth(_T("返回主菜单"));
    outtextxy((WIN_W - btnTextW) / 2, btn.top + 14, _T("返回主菜单"));

    // 5. 处理鼠标点击：while消费全部鼠标消息，标记变量后再执行逻辑
    ExMessage msg;
    bool clickReturn = false;
    while (peekmessage(&msg, EX_MOUSE))
    {
        if (msg.message == WM_LBUTTONDOWN)
        {
            if (msg.x >= btn.left && msg.x <= btn.right &&
                msg.y >= btn.top && msg.y <= btn.bottom)
            {
                clickReturn = true;
            }
        }
    }
    if (clickReturn)
    {
        clearArrays();          // 清空当前游戏数据
        state = STATE_MENU;     // 回到主菜单
    }

    // ---- 恢复绘图状态，完整还原进入函数之前的全部绘图设置 ----
    setfillcolor(oldFill);
    setlinecolor(oldLine);
    settextcolor(oldText);
    setbkmode(oldBkMode);
    setfont(&oldFont);

    return;

}
//==============================================================================================















//嘉豪8.24
//==============================================================================================
/*
 * 负责人：嘉豪
 * 功能：失败结算界面
 *   1. 绘制背景和半透明遮罩
 *   2. 显示失败标题
 *   3. 显示统计信息（坚持时长、消灭敌人数）
 *   4. 返回主菜单按钮
 *   5. 鼠标点击 / Esc 返回主菜单
 * 参数：无
 * 返回值：无
 * 涉及全局变量：state、gameSeconds、killedCount
 * 注意：
 *   - 单帧函数，不阻塞主循环
 *   - 输入统一用 peekmessage 消费本帧全部消息，用标记变量延迟修改 state
 *   - 半透明遮罩需 initgraph 开启 DOUBLEBUF，否则可改用多层黑色叠加
 *   - 函数开头保存绘图状态（颜色、背景模式、字体），结束前全部恢复
 */
void defeatView() {
    cleardevice();
    drawBackground();
    // TODO: 显示失败信息和统计

    // ---- 保存当前绘图状态 ----
    COLORREF oldFill = getfillcolor();
    COLORREF oldLine = getlinecolor();
    COLORREF oldText = gettextcolor();
    int oldBkMode = getbkmode();
    LOGFONT oldFont;
    getfont(&oldFont);
    cleardevice();
    drawBackground();
    // 半透明遮罩（ARGB；若未启用 DOUBLEBUF，会显示为不透明黑色）
    setfillcolor(0x88000000);
    solidrectangle(0, 0, WIN_W, WIN_H);
    setbkmode(TRANSPARENT);
    // 失败标题（红字居中，使用英文半角逗号）
    settextstyle(60, 0, _T("宋体"));
    settextcolor(RED);
    const TCHAR* title = _T("Defeat, 皮丹阵亡");
    int titleW = textwidth(title);
    outtextxy((WIN_W - titleW) / 2, 120, title);
    // 统计信息（分两行居中）
    settextstyle(30, 0, _T("宋体"));
    settextcolor(WHITE);
    TCHAR line1[128];
    _stprintf_s(line1, _countof(line1), _T("坚持时长：%d 秒"), gameSeconds);
    int w1 = textwidth(line1);
    outtextxy((WIN_W - w1) / 2, 240, line1);
    TCHAR line2[128];
    _stprintf_s(line2, _countof(line2), _T("消灭敌人：%d 个"), killedCount);
    int w2 = textwidth(line2);
    outtextxy((WIN_W - w2) / 2, 300, line2);
    // 返回主菜单按钮（居中）
    int btnW = 240;
    int btnH = 60;
    int btnX = (WIN_W - btnW) / 2;
    int btnY = 400;
    setfillcolor(RGB(80, 80, 80));
    setlinecolor(RGB(200, 200, 200));
    fillrectangle(btnX, btnY, btnX + btnW, btnY + btnH);
    settextstyle(28, 0, _T("宋体"));
    settextcolor(WHITE);
    const TCHAR* btnText = _T("返回主菜单");
    int btnTw = textwidth(btnText);
    outtextxy(btnX + (btnW - btnTw) / 2, btnY + 14, btnText);
    // 输入处理（消费本帧全部鼠标/键盘消息，使用标记变量）
    ExMessage msg;
    bool shouldExit = false;
    while (peekmessage(&msg, EX_MOUSE | EX_KEY)) {
        if (msg.message == WM_LBUTTONDOWN) {
            if (msg.x >= btnX && msg.x <= btnX + btnW &&
                msg.y >= btnY && msg.y <= btnY + btnH) {
                shouldExit = true;
            }
        }
        if (msg.message == WM_KEYDOWN && msg.vkcode == VK_ESCAPE) {
            shouldExit = true;
        }
    }
    if (shouldExit) {
        clearArrays();          // 清空当前游戏数据
        state = STATE_MENU;     // 回到主菜单
    }
    // ---- 恢复绘图状态（不硬编码字体，恢复进入前的状态）----
    setfillcolor(oldFill);
    setlinecolor(oldLine);
    settextcolor(oldText);
    setbkmode(oldBkMode);
    setfont(&oldFont);
}
//==============================================================================================








//Three8.23
//==============================================================================================
/*
 * 负责人：three
 * 功能：暂停界面显示与交互（继续 / 返回主菜单），Esc 切换
 * 参数：无
 * 返回值：无
 * 涉及全局变量：state
 */
void pauseView() {
    cleardevice();
    drawBackground();
    // TODO: 显示暂停菜单


    setbkmode(TRANSPARENT);

    // 深色遮罩
    setfillcolor(RGB(10, 10, 10));
    fillrectangle(0, 0, WIN_W, WIN_H);

    // 标题
    settextcolor(RGB(212, 160, 23));
    settextstyle(48, 0, _T("黑体"));
    const TCHAR* title = _T("游戏暂停");
    outtextxy((WIN_W - textwidth(title)) / 2, 160, title);

    // 按钮通用样式
    const int BTN_W = 240, BTN_H = 60;
    const int BTN_X = (WIN_W - BTN_W) / 2;
    const int CONT_BTN_Y = 280, HOME_BTN_Y = 360;

    // 继续游戏按钮
    setfillcolor(RGB(26, 26, 26));
    fillrectangle(BTN_X, CONT_BTN_Y, BTN_X + BTN_W, CONT_BTN_Y + BTN_H);
    setlinecolor(WHITE);
    rectangle(BTN_X, CONT_BTN_Y, BTN_X + BTN_W, CONT_BTN_Y + BTN_H);
    settextcolor(RGB(255, 255, 255));
    settextstyle(28, 0, _T("黑体"));
    const TCHAR* contText = _T("继续游戏");
    outtextxy(BTN_X + (BTN_W - textwidth(contText)) / 2,
        CONT_BTN_Y + (BTN_H - textheight(contText)) / 2, contText);

    // 返回主菜单按钮
    setfillcolor(RGB(26, 26, 26));
    fillrectangle(BTN_X, HOME_BTN_Y, BTN_X + BTN_W, HOME_BTN_Y + BTN_H);
    rectangle(BTN_X, HOME_BTN_Y, BTN_X + BTN_W, HOME_BTN_Y + BTN_H);
    const TCHAR* homeText = _T("返回主菜单");
    outtextxy(BTN_X + (BTN_W - textwidth(homeText)) / 2,
        HOME_BTN_Y + (BTN_H - textheight(homeText)) / 2, homeText);

    // 底部提示
    settextcolor(RGB(200, 200, 200));
    settextstyle(16, 0, _T("黑体"));
    const TCHAR* escTip = _T("按 Esc 继续");
    outtextxy((WIN_W - textwidth(escTip)) / 2, HOME_BTN_Y + BTN_H + 30, escTip);

    // 输入处理
    ExMessage msg;
    while (peekmessage(&msg, EX_MOUSE | EX_KEY)) {
        if (msg.message == WM_KEYDOWN && msg.vkcode == VK_ESCAPE) {
            state = STATE_GAME;
            goto PAUSE_EXIT; // 跳转到末尾做绘图状态复原
        }
        if (msg.message == WM_LBUTTONDOWN) {
            if (msg.x >= BTN_X && msg.x <= BTN_X + BTN_W &&
                msg.y >= CONT_BTN_Y && msg.y <= CONT_BTN_Y + BTN_H) {
                state = STATE_GAME;
                goto PAUSE_EXIT;
            }
            if (msg.x >= BTN_X && msg.x <= BTN_X + BTN_W &&
                msg.y >= HOME_BTN_Y && msg.y <= HOME_BTN_Y + BTN_H) {
                state = STATE_MENU;
                goto PAUSE_EXIT;
            }
        }
    }
PAUSE_EXIT:
    // 复原绘图状态，防止污染其他view界面
    settextcolor(BLACK);
    settextstyle(20, 0, _T("宋体"));
    setfillcolor(WHITE);
    setlinecolor(BLACK);
}
//==============================================================================================












void settingView() {
    cleardevice();
    drawBackground();
    // TODO: 设置界面
}

void saveManageView() {
    cleardevice();
    drawBackground();
    // TODO: 存档管理
}

void levelSelectView() {
    cleardevice();
    drawBackground();
    // TODO: 关卡选择
}















//8.24寻光
//==============================================================

// ------------------------------------------------------------
// 团队介绍模块  负责人：寻光
// 说明：团队介绍界面（UI-09）：
//   1. 像素坐标分列展示成员（姓名/角色/职位/负责模块），像素对齐，不受汉字宽度影响
//   2. 表格外显示丹丹的奖状图片（放 exe 同目录 res/award.jpg）
//   3. 底部"返回主菜单"按钮
//   单帧函数，由主循环调度，内部禁止 while(1)。
//   使用架构已有 team[] 数组，不重复维护成员数据
// ------------------------------------------------------------

// 返回按钮尺寸
#define TEAM_BACK_W 200
#define TEAM_BACK_H 50
static int teamBackX, teamBackY;

// 表格每一列的X像素坐标
#define COL_NAME_X     120
#define COL_ROLE_X     240
#define COL_POS_X      360
#define COL_MODULE_X   480

/*
 * 负责人：寻光
 * 功能：绘制团队成员表格 + 返回按钮
 * 参数：无
 * 返回值：无
 * 涉及全局变量：team[]、teamCount（架构已有）
 */
static void drawTeamIntro() {
    // 右上角标题
    settextstyle(28, 0, _T("宋体"));
    settextcolor(WHITE);
    setbkmode(TRANSPARENT);
    outtextxy(WIN_W - textwidth(_T("团队介绍")) - 30, 25, _T("团队介绍"));

    int startY = 100;
    int rowH = 30;
    settextstyle(18, 0, _T("宋体"));
    settextcolor(WHITE);

    // 表头：每一列独立X，像素对齐
    outtextxy(COL_NAME_X, startY, _T("姓名"));
    outtextxy(COL_ROLE_X, startY, _T("角色"));
    outtextxy(COL_POS_X, startY, _T("职位"));
    outtextxy(COL_MODULE_X, startY, _T("负责模块"));

    // 绘制每一行成员
    for (int i = 0; i < teamCount; i++) {
        int y = startY + (i + 1) * rowH;
        outtextxy(COL_NAME_X, y, team[i].name);
        outtextxy(COL_ROLE_X, y, team[i].role);
        outtextxy(COL_POS_X, y, team[i].position);
        outtextxy(COL_MODULE_X, y, team[i].module);
    }

    // 底部返回主菜单按钮
    teamBackX = (WIN_W - TEAM_BACK_W) / 2;
    teamBackY = WIN_H - TEAM_BACK_H - 40;

    setfillcolor(RGB(70, 130, 240));
    solidrectangle(teamBackX, teamBackY, teamBackX + TEAM_BACK_W, teamBackY + TEAM_BACK_H);
    setlinecolor(RGB(0, 0, 0));
    rectangle(teamBackX, teamBackY, teamBackX + TEAM_BACK_W, teamBackY + TEAM_BACK_H);

    settextstyle(20, 0, _T("宋体"));
    settextcolor(BLACK);
    int tw = textwidth(_T("返回主菜单"));
    int th = textheight(_T("返回主菜单"));
    outtextxy(teamBackX + (TEAM_BACK_W - tw) / 2,
        teamBackY + (TEAM_BACK_H - th) / 2,
        _T("返回主菜单"));
}

/*
 * 负责人：寻光
 * 功能：团队介绍界面（UI-09）
 *   1. 绘制标题、表格、返回按钮
 *   2. 加载并显示丹丹奖状图片（exe同目录 res/award.jpg）
 *   3. 加载失败界面文字提示，不弹MessageBox，不阻塞循环
 *   4. 鼠标点击返回按钮切回主菜单 state = STATE_MENU
 * 参数：无
 * 返回值：无
 * 涉及全局变量：state
 * 注意：单帧函数，由主循环调度，内部禁止 while(1)
 */



void teamIntroView() {
    cleardevice();
    drawBackground();
    // TODO: 团队介绍

    drawTeamIntro();

    // ----奖状图片加载，EasyX loadimage为void无返回值----
    static bool imgTried = false;
    static IMAGE awardImg;
    static bool awardLoaded = false;

    if (!imgTried) {
        imgTried = true;
        // _tfopen 兼容 _T 宏，适配Unicode/多字节双字符集
        FILE* fp = _tfopen(_T("res/award.jpg"), _T("rb"));
        if (fp != NULL) {
            fclose(fp);
            loadimage(&awardImg, _T("res/award.jpg"), 300, 200);
            awardLoaded = true;
        }
        else {
            awardLoaded = false;
        }
    }

    if (awardLoaded) {
        putimage(120, 380, &awardImg);
    }
    else {
        settextcolor(RGB(255, 215, 0));
        settextstyle(18, 0, _T("宋体"));
        // 根据窗口宽度计算提示文字X，窗口改尺寸也不会错位
        const TCHAR* tipStr = _T("[奖状资源缺失]");
        int tipX = WIN_W - textwidth(tipStr) - 30;
        outtextxy(tipX, 150, tipStr);
    }

    // 鼠标交互：点击返回按钮（和项目其余view保持一致，视图内部处理消息）
    ExMessage msg;
    while (peekmessage(&msg, EX_MOUSE)) {
        if (msg.message == WM_LBUTTONDOWN) {
            if (msg.x >= teamBackX && msg.x <= teamBackX + TEAM_BACK_W &&
                msg.y >= teamBackY && msg.y <= teamBackY + TEAM_BACK_H) {
                state = STATE_MENU;
                break;
            }
        }
    }

    //追加键盘，支持Esc返回
    while (peekmessage(&msg, EX_KEY)) {
        if (msg.message == WM_KEYDOWN && msg.vkcode == VK_ESCAPE) {
            state = STATE_MENU;
        }
    }


    // 还原EasyX绘图全局状态，防止污染其他界面
    setfillcolor(WHITE);
    setlinecolor(BLACK);
    settextcolor(BLACK);
    settextstyle(20, 0, _T("宋体"));
    setbkmode(OPAQUE);

}
//==============================================================================================





















void gameSettingView() {
    cleardevice();
    drawBackground();
    // TODO: 游戏内设置
}

// ==================================================================================
// 逻辑层实现（占位）
// ==================================================================================







//蛋总8.21
//====================================================================================
/*
 负责人：丹丹
 功能：初始化新游戏数据
    1. 重置所有核心全局变量（金币、血量、生成数、核弹冷却等）
    2. 对 currentLevel 做越界防护
    3. 调用 clearArrays() 清空所有单位/敌人/箭矢数组
    4. 将游戏状态设置为 STATE_GAME
 参数：无
 返回值：无
 涉及全局变量：gold、baseHp、baseMaxHp、totalSpawned、killedCount、
              bombCooldown、bombAvailable、gameTime、gameSeconds、
              unitCount[]、currentLevel、state
 */
void initGame() {
    // TODO:
    // 1. 重置核心变量
    //    gold = 50; baseHp = 100; baseMaxHp = 100;
    //    totalSpawned = 0; killedCount = 0;
    //    bombCooldown = 0; bombAvailable = true;
    //    gameTime = 0; gameSeconds = 0;
    //    for (i) unitCount[i] = 0;
    // 2. 防护 currentLevel 越界：
    //    if (currentLevel < 1 || currentLevel > levelCount) currentLevel = 1;
    // 3. 清空所有数组 clearArrays()
    // 4. state = STATE_GAME;


    // 1. 重置核心变量:
    gold = 50;              // 初始金币
    baseMaxHp = 100;        // 大本营最大血量
    baseHp = 100;           // 大本营当前血量
    totalSpawned = 0;       // 已生成敌人总数
    killedCount = 0;        // 已消灭敌人数
    bombCooldown = 0;       // 核弹冷却剩余帧数
    bombAvailable = true;   // 核弹是否可用
    gameTime = 0;           // 游戏帧计数
    gameSeconds = 0;        // 游戏秒数（gameTime 每满 FPS 帧 +1）
    // 各职业存活数量全部归零
    for (int i = 0; i < UT_TYPE_COUNT; i++)
    {
        unitCount[i] = 0;
    }
    //2.防护 currentLevel 越界:
    // currentLevel 从 1 开始，levelCount 是关卡配置总数。
    // 如果被改成了非法值（如 0 或 99），强制回到第 1 关。
    if (currentLevel < 1 || currentLevel > levelCount)
    {
        currentLevel = 1;
    }
    //3. 清空所有数组 clearArrays():
    clearArrays();
    //4.切换状态到游戏主界面:
    state = STATE_GAME;
}
//====================================================================================







//dandan8.22
//======================================================================================
/*
 * 负责人：丹丹
 * 功能：购买单位并生成到场上
 *   - 校验参数、金币、上限、容量
 *   - 扣除金币，unitCount[type]++（单位成功入数组后更新计数）
 *   - 根据类型生成单位并设置坐标和属性
 *   - 盾卫购买后增加大本营最大血量和当前血量
 * 参数：type - 单位类型（UnitType）
 * 返回值：true-成功；false-失败
 * 涉及全局变量：gold、unitCount[]、baseHp、baseMaxHp、myUnits、myUnitCount
 */
bool buyUnit(int type) {
    // 1.参数防护：非法单位类型
    if (type < 0 || type >= UT_TYPE_COUNT) return false;

    // TODO:
    // 1. 检查金币 gold >= unitPrice[type]
    // 2. 检查上限 unitCount[type] < unitLimit[type]
    // 3. 扣除金币 gold -= unitPrice[type];
    //    unitCount[type]++;
    // 4. 生成单位：
    //    - 矿工(UT_MINER)    → 矿区（MINE1_X/MINE2_X 附近）
    //    - 盾卫(UT_SHIELD)   → SHIELD_X
    //    - 弓箭手(UT_ARCHER) → ARCHER_X
    //    - 将士/将军          → 大本营(BASE_X, BASE_Y) 出发
    // 5. 【盾卫特殊】if (type == UT_SHIELD) baseMaxHp += 10;
    //    （是否需要 baseHp 也 +10，小组自定，注释写明）
    


    // 2. 容量防护：我方单位总数已达上限（防止越界）
    if (myUnitCount >= MY_UNIT_MAX) {
        return false;
    }

    // 3. 检查金币
    if (gold < unitPrice[type]) {
        return false; // 金币不足，购买失败
    }

    // 4. 检查该类型单位购买数量上限
    if (unitCount[type] >= unitLimit[type]) {
        return false; // 数量达上限，购买失败
    }

    // 5. 扣除金币（资源先扣，计数延后，防止数组写入失败造成统计不一致）
    gold -= unitPrice[type];

    // 6. 创建新单位并填入基础属性
    Unit u;
    u.type = type;                    // 单位类型
    u.alive = true;                  // 存活
    u.maxHp = unitHp[type];          // 最大血量
    u.hp = unitHp[type];             // 当前血量
    u.atk = unitAtk[type];           // 攻击力
    u.speed = unitSpeed[type];       // 移动速度
    u.cd = 0;                        // 冷却计时从0开始（进场即可行动）
    u.cdMax = 0;                     // 冷却上限，按类型在下面设置

    // 7. 按类型设置出生坐标和冷却
    switch (type) {
    case UT_MINER:
        // 矿工 → 矿区，上下两个矿区交替放置
        // 即将新增第 (unitCount[type]+1) 个该类型矿工
        if ((unitCount[type] + 1) % 2 == 1) {
            u.x = MINE1_X;
            u.y = MINE1_Y;
        }
        else {
            u.x = MINE2_X;
            u.y = MINE2_Y;
        }
        u.cdMax = MINER_PRODUCE_FRAME;   // 与 updateMiners() 生产间隔一致
        break;

    case UT_SHIELD:
        // 盾卫 → 大本营前方，多个盾卫向右排开
        u.x = SHIELD_X + (unitCount[type]) * 20;
        u.y = SHIELD_Y;
        u.cdMax = 0;                     // 盾卫不主动攻击，无冷却

        // 盾卫特殊效果：购买时大本营最大血量+10，当前血量也+10
        // 受 unitLimit[UT_SHIELD] 上限约束，不能无限购买
        baseMaxHp += 10;
        baseHp += 10;
        break;

    case UT_ARCHER:
        // 弓箭手 → 固定站位，多个弓箭手向右排开
        u.x = ARCHER_X + (unitCount[type]) * 15;
        u.y = ARCHER_Y;
        u.cdMax = ARCHER_CD_FRAME;       // 射击冷却
        u.cd = 0;                        // 进场即可射击
        break;

    case UT_WARRIOR:
    case UT_GENERAL:
        // 将士/将军 → 从大本营位置出发
        // 将士、将军各自独立计数，位置互不干扰
        u.x = BASE_X + (unitCount[type]) * 10;
        u.y = BASE_Y;
        u.cdMax = WARRIOR_CD_FRAME;      // 近战攻击冷却
        u.cd = 0;                        // 进场即可攻击
        break;

    default:
        return false;
    }

    // 8. 把新单位放入数组，更新全局计数
    if (myUnitCount >= MY_UNIT_MAX) {
        return false;   // 数组已满，拒绝写入
    }
    myUnits[myUnitCount] = u;
    myUnitCount++;
    unitCount[type]++;   // ✅挪至此：真正存入数组之后，再增加类型计数

    return true; // 购买成功
}
//======================================================================================









//Three8.21
//======================================================================================
/*
 * 负责人：Three
 * 功能：更新所有存活矿工的产金币行为
 *   每 MINER_PRODUCE_FRAME（40帧）帧，每个存活矿工产出 1 金币
 *   冷却未到时递减，冷却清零时产出金币并重置冷却
 * 参数：无
 * 返回值：无
 * 涉及全局变量：myUnits[]、myUnitCount、gold、state
 */
void updateMiners() {
    // 每 MINER_PRODUCE_FRAME 帧，存活矿工 +1 金币
    // 注意：使用 gameTime % MINER_PRODUCE_FRAME == 0 判断

    if (state != STATE_GAME) return;
    for (int i = 0; i < myUnitCount; i++) {
        Unit* u = &myUnits[i];
        if (!u->alive || u->type != UT_MINER) continue;
        if (u->cd > 0) {
            u->cd--;
        }
        else {
            gold += 1;
            u->cd = MINER_PRODUCE_FRAME;   // 40帧
        }
    }
}
//======================================================================================










//嘉豪8.21
//======================================================================================
/*
负责人：嘉豪
功能：按关卡配置生成敌人
参数：无
返回值：无
涉及全局变量：currentLevel, totalSpawned, gameTime, enemies, enemyCount
*/
void spawnEnemy() {
    // TODO:
    // 1. 获取关卡配置：int idx = currentLevel - 1;
    //    if (idx < 0 || idx >= levelCount) return;
    // 2. 判断是否生成完：if (totalSpawned >= levelConfigs[idx].totalEnemies) return;
    // 3. 生成时机：if (gameTime % levelConfigs[idx].spawnInterval != 0) return;
    // 4. 随机生成敌人类型（按权重），添加到 enemies 数组
    //    出生点 (SPAWN_X, PATH_Y)，dx = -1
    //    血量 = enemyBaseHp[type] * levelConfigs[idx].hpMul
    //    攻击 = enemyBaseAtk[type] * levelConfigs[idx].atkMul
    //    totalSpawned++;


     // 获取当前关卡配置（防止越界）
    int idx = currentLevel - 1;
    if (idx < 0 || idx >= levelCount) return;
    LevelConfig* cfg = &levelConfigs[idx];
    // 达到本关总敌数后不再生成
    if (totalSpawned >= cfg->totalEnemies) return;
    // 判断是否到了生成间隔（帧）
    if (gameTime % cfg->spawnInterval != 0) return;
    // 敌人数组已满则不再生成（防止越界）
    if (enemyCount >= ENEMY_MAX) return;
    // 按权重随机选择敌人类型
    int sumWeight = cfg->weightVar + cfg->weightArr + cfg->weightPtr;
    int randVal = rand() % sumWeight;
    int enemyType;
    if (randVal < cfg->weightVar) {
        enemyType = ET_VARIABLE;
    }
    else {
        randVal -= cfg->weightVar;
        if (randVal < cfg->weightArr) {
            enemyType = ET_ARRAY;
        }
        else {
            enemyType = ET_POINTER;
        }
    }
    // 创建敌人并放入数组
    enemies[enemyCount].type = enemyType;
    enemies[enemyCount].x = SPAWN_X;
    enemies[enemyCount].y = PATH_Y;
    enemies[enemyCount].dx = -1;
    enemies[enemyCount].hp = (int)(enemyBaseHp[enemyType] * cfg->hpMul);
    enemies[enemyCount].maxHp = enemies[enemyCount].hp;
    enemies[enemyCount].atk = (int)(enemyBaseAtk[enemyType] * cfg->atkMul);
    enemies[enemyCount].speed = enemyBaseSpeed[enemyType];
    enemies[enemyCount].cd = 0;
    enemies[enemyCount].cdMax = ENEMY_ATK_CD_MAX;
    enemies[enemyCount].alive = true;
    enemyCount++;
    totalSpawned++;
}
//======================================================================================








//嘉豪8.22
//======================================================================================
/*
 * 负责人：嘉豪
 * 功能：更新所有存活敌人的行为
 * 逻辑说明：
 * 1. 遍历全部敌人，跳过已经死亡的敌人
 * 2. 若敌人x坐标小于等于基地横坐标，敌人抵达大本营，对基地造成伤害并标记自身死亡
 * 3. 敌人攻击冷却每帧固定递减，移动过程冷却也正常计时
 * 4. 遍历查找范围内任意存活的我方单位作为攻击目标，不区分单位类型，矿工、战斗单位均可被选为目标
 * 5. 找到有效目标：冷却完毕则执行攻击，扣除目标血量；目标血量归零时标记死亡，同步我方单位计数
 * 6. 未找到攻击目标：敌人向左移动，朝大本营方向前进
 * 参数依赖：
 *   enemyCount      敌人总数量
 *   enemies[]       敌人结构体数组
 *   myUnitCount     我方单位总数量
 *   myUnits[]       我方单位结构体数组
 *   baseHp          基地当前血量
 *   unitCount[]     各类我方存活单位计数数组
 * 宏依赖：
 *   ENEMY_ATK_RANGE_SQ   敌人攻击距离平方
 *   ENEMY_ATK_CD_MAX     敌人攻击冷却最大帧数
 *   BASE_X               大本营横坐标
 */
void updateEnemies() {
    // TODO: 敌人左移，遇单位攻击，到达BASE_X扣baseHp
    // 敌人死亡时：alive = false; enemyCount 不减（稀疏数组）


    for (int i = 0; i < enemyCount; i++) {
        Enemy* enemy = &enemies[i];
        if (!enemy->alive) continue;

        // 到达大本营
        if (enemy->x <= BASE_X) {
            baseHp -= enemy->atk;
            enemy->alive = false;
            continue;
        }

        //【改动】无论是否找到目标，攻击冷却每一帧都递减（移动过程也转CD）
        if (enemy->cd > 0)
        {
            enemy->cd--;
        }

        // 寻找攻击范围内的任何我方存活单位
        bool hasTarget = false;
        int targetIdx = -1;
        for (int j = 0; j < myUnitCount; j++) {
            Unit* u = &myUnits[j];
            if (!u->alive) continue;
            int dx = enemy->x - u->x;
            int dy = enemy->y - u->y;
            int distSq = dx * dx + dy * dy;
            if (distSq < ENEMY_ATK_RANGE_SQ) {
                hasTarget = true;
                targetIdx = j;
                break;
            }
        }

        if (hasTarget) {
            Unit* target = &myUnits[targetIdx];
            if (enemy->cd <= 0)
            {
                target->hp -= enemy->atk;
                enemy->cd = enemy->cdMax;

                //【修复】只有原本活着，现在致死，才扣unitCount，防止多次自减
                if (target->hp <= 0 && target->alive) {
                    target->alive = false;
                    unitCount[target->type]--;
                }
            }
        }
        else {
            // 无目标，向前推进
            enemy->x -= enemy->speed;
        }
    }
}
//======================================================================================












//KAKA8.22
//========================================================================
/*
 * 负责人：卡卡
 * 功能：更新所有存活弓箭手的攻击行为
 *   1. 遍历我方单位数组，找出类型为弓箭手且存活的目标
 *   2. 冷却结束后，调用 findLeftmostEnemy() 索敌
 *   3. 找到敌人后，在弓箭手前方生成箭矢，并重置冷却
 *   4. 冷却未结束则递减冷却计时
 * 参数：无
 * 返回值：无
 * 涉及数据：myUnits[]（Unit数组）、arrows[]（Arrow数组）、
 *           myUnitCount、arrowCount、ARCHER_CD_FRAME、
 *           findLeftmostEnemy()、UT_ARCHER
 */
void updateArchers() {
    // TODO: 弓箭手 cd <= 0 时，findLeftmostEnemy() 找目标
    // 发射箭矢，重置 cd = ARCHER_CD_FRAME（30帧）


    for (int i = 0; i < myUnitCount; i++) {
        // 只处理存活且为弓箭手的单位
        if (myUnits[i].alive && myUnits[i].type == UT_ARCHER) {
            // 冷却完毕
            if (myUnits[i].cd <= 0) {
                // 索敌：最左侧存活敌人
                int targetIdx = findLeftmostEnemy();
                if (targetIdx != -1) {
                    // 仅当箭矢数组未满时才发射，避免数组越界
                    if (arrowCount < ARROW_MAX) {
                        // 创建箭矢：从弓箭手正面（右侧）发出
                        arrows[arrowCount].x = myUnits[i].x + 20;
                        arrows[arrowCount].y = myUnits[i].y;
                        arrows[arrowCount].dx = 1;
                        arrows[arrowCount].alive = true;
                        arrowCount++;
                        // 只有发射成功才重置冷却
                        myUnits[i].cd = ARCHER_CD_FRAME;
                    }
                    // 如果数组已满，本帧发射失败，不重置冷却，下帧继续尝试
                }
                // 如果找不到敌人，不修改 cd，保持可用状态，等待敌人出现后立即射击
            }
            else {
                myUnits[i].cd--;   // 冷却递减
            }
        }
    }
}
//========================================================================









//KAKA8.23
//========================================================================
/*
 * 负责人：卡卡
 * 功能：更新所有存活箭矢的移动与碰撞
 *   1. 箭矢向右移动（x += ARROW_SPEED）
 *   2. 飞出屏幕右边界（x > WIN_W）则标记为死亡（alive = false）
 *   3. 与存活敌人进行碰撞检测（距离平方 < 400 即半径 < 20）
 *   4. 命中敌人：扣除敌人血量（弓箭手攻击力15），
 *      敌人死亡则标记为死亡并增加 killedCount
 *   5. 箭矢命中后自身标记为死亡
 *   6. 调用结束后进行内存压缩，移除数组中的死亡箭矢
 * 参数：无
 * 返回值：无
 * 涉及数据：arrows[]、enemies[]、arrowCount、enemyCount、
 *           winner、killedCount、unitAtk[UT_ARCHER]、
 *           WIN_W、ARROW_SPEED
 */
void updateArrows() {
    // TODO: 箭矢移动、碰撞、伤害
    // 飞出屏幕(WIN_W)回收

     // 遍历箭矢数组（使用 while 循环以便进行压缩操作）
    for (int i = 0; i < arrowCount; ) {
        if (!arrows[i].alive) {
            // 如果箭矢已死亡，则用数组最后一个元素覆盖当前，
            /*
            数组压缩：交换并缩减策略（箭矢）
            数组：arrows[ARROW_MAX]，ARROW_MAX=50；arrowCount代表当前有效箭矢数量。
            背景：
            箭矢命中敌人 / 飞出屏幕 → 设置 arrows[i].alive = false
            仅仅标记死亡，数组位置还占着，arrowCount不会自己变小。
            如果一直只标记不清理，很快 arrowCount 冲到 50，再也射不出新箭。
            普通思路：把后面全部元素往前挪（移位删除），效率低。
            交换‑缩减策略（快）：用数组最后一个有效元素覆盖死亡位置，然后 arrowCount‑‑
            */
            // 并减少计数（压缩数组），注意不执行 i++
            arrows[i] = arrows[arrowCount - 1];
            arrowCount--;
            continue; // 重新检查当前位置（交换过来的元素）
        }
        // 1. 移动箭矢
        arrows[i].x += ARROW_SPEED;
        // 2. 飞出屏幕右边界则销毁
        if (arrows[i].x > WIN_W) {
            arrows[i].alive = false;
            continue; // 跳到循环顶部，等待压缩
        }
        // 3. 碰撞检测（遍历敌人）
        for (int j = 0; j < enemyCount; j++) {
            if (!enemies[j].alive) continue; // 跳过死亡敌人
            // 计算箭矢与敌人的距离平方
            int dx = arrows[i].x - enemies[j].x;
            int dy = arrows[i].y - enemies[j].y;
            int distSq = dx * dx + dy * dy;
            // 如果距离小于 20 像素，判定为命中
            if (distSq < ARROW_HIT_RADIUS_SQ) {
                // 4. 造成伤害（弓箭手基础攻击力）
                enemies[j].hp -= unitAtk[UT_ARCHER];
                // 5. 敌人死亡时更新状态和消灭计数
                if (enemies[j].hp <= 0) {
                    enemies[j].alive = false;
                    killedCount++;
                    // 注意：对应职业数量在此处不递减，因为击杀敌人不涉及我方单位
                }
                // 6. 箭矢消失
                arrows[i].alive = false;
                break; // 跳出敌人循环，箭矢已消失，不再检测其他敌人
            }
        }
        // PLUS命中后立即回到循环头部执行压缩
        if (!arrows[i].alive) {
            continue;
        }
        // 移动 i 指针，继续处理下一个元素（未命中时，移动到下一个箭矢）
        i++;
    }
}
//========================================================================








//xunguang8.22
//========================================================================
/*
 * 负责人：寻光
 * 功能：更新所有存活将士/将军的移动与攻击行为
 * 参数：无
 * 返回值：无
 */
void updateSoldiers() {
    // TODO: 将士/将军右移、遇敌攻击
    // 死亡时：alive = false; unitCount[type]--;
    // 注意不要重复递减


    for (int i = 0; i < myUnitCount; i++) {
        if (!myUnits[i].alive) continue;
        if (myUnits[i].type != UT_WARRIOR && myUnits[i].type != UT_GENERAL) continue;
        if (myUnits[i].cd > 0) myUnits[i].cd--;
        // 直接遍历敌人寻找目标
        bool foundEnemy = false;
        for (int j = 0; j < enemyCount; j++) {
            if (!enemies[j].alive) continue;
            int dx = enemies[j].x - myUnits[i].x;
            int dy = enemies[j].y - myUnits[i].y;
            if (dx * dx + dy * dy <= SOLDIER_ATK_RANGE * SOLDIER_ATK_RANGE) {
                foundEnemy = true;
                if (myUnits[i].cd <= 0) {
                    enemies[j].hp -= myUnits[i].atk;
                    if (enemies[j].hp <= 0) {
                        enemies[j].alive = false;
                        killedCount++;   // 之前遗漏
                    }
                    myUnits[i].cd = (myUnits[i].cdMax > 0) ? myUnits[i].cdMax : 20;
                }
                break;
            }
        }
        if (!foundEnemy) {
            myUnits[i].x += myUnits[i].speed;
            if (myUnits[i].x > SPAWN_X) myUnits[i].x = SPAWN_X;
        }
        // 死亡处理（加 alive 防护）
        if (myUnits[i].alive && myUnits[i].hp <= 0) {
            myUnits[i].alive = false;
            unitCount[myUnits[i].type]--;
        }
    }
}
//========================================================================







//8.23寻光
//========================================================================
/*
 * 负责人：寻光
 * 功能：核弹答题界面（分帧版，由 useBomb 进入后每帧调用）
 *   1. 绘制题目弹窗和四个选项
 *   2. 用 peekmessage 统一检测鼠标/键盘输入
 *   3. 回答后：
 *       答对 → clearEnemies() + 爆炸特效 + 开启核弹冷却
 *       答错 → 禁用核弹（bombAvailable = false）
 *       取消（Esc）→ 不改变任何状态
 *   4. 结束答题：isBombAnswering = false
 * 参数：无
 * 返回值：bool（true 表示答对，false 表示答错/取消/未完成）
 * 涉及全局变量：isBombAnswering、currentBombQIdx、bombCooldown、
 *               bombAvailable、bombEffectActive 等
 * 注意：本函数不检测空格，由 useBomb() 负责触发。
 */
bool askQuestion() {
    // TODO: 随机抽题绘制，等待选择

    // 如果不在答题状态，直接返回
    if (!isBombAnswering) {
        return false;
    }
    // 半透明黑色遮罩（50%透明度，ARGB格式）
    setfillcolor(0x80000000);
    solidrectangle(0, 0, WIN_W, WIN_H);

    int boxW = 500;
    int boxH = 320;
    int boxX = (WIN_W - boxW) / 2;
    int boxY = (WIN_H - boxH) / 2;
    // 弹窗主体
    setfillcolor(RGB(45, 45, 55));
    solidroundrect(boxX, boxY, boxX + boxW, boxY + boxH, 10, 10);
    setlinecolor(RGB(200, 200, 220));
    setlinestyle(PS_SOLID, 2);
    roundrect(boxX, boxY, boxX + boxW, boxY + boxH, 10, 10);
    setlinestyle(PS_SOLID, 1);
    setbkmode(TRANSPARENT);
    settextcolor(RGB(255, 215, 0));
    settextstyle(24, 0, _T("黑体"));
    outtextxy(boxX + 20, boxY + 20, _T("核弹激活验证"));
    settextcolor(WHITE);
    settextstyle(16, 0, _T("黑体"));
    outtextxy(boxX + 20, boxY + 70, questions[currentBombQIdx].text);

    int optW = boxW - 40;
    int optH = 40;
    int optX = boxX + 20;
    int optStartY = boxY + 120;
    int optGap = 50;
    for (int i = 0; i < 4; i++) {
        int oy = optStartY + i * optGap;
        setfillcolor(RGB(70, 70, 90));
        solidroundrect(optX, oy, optX + optW, oy + optH, 6, 6);
        setlinecolor(RGB(150, 150, 180));
        roundrect(optX, oy, optX + optW, oy + optH, 6, 6);
        settextcolor(WHITE);
        settextstyle(14, 0, _T("黑体"));
        TCHAR buf[80];
        _stprintf_s(buf, _countof(buf), _T("%d. %s"), i + 1, questions[currentBombQIdx].options[i]);
        outtextxy(optX + 15, oy + 12, buf);
    }

    // 输入处理（统一用 peekmessage）
    ExMessage msg;
    bool answered = false;
    bool correct = false;
    bool cancel = false;
    while (peekmessage(&msg, EX_MOUSE | EX_KEY)) {
        // 按 Esc 取消答题
        if (msg.message == WM_KEYDOWN && msg.vkcode == VK_ESCAPE) {
            answered = true;
            cancel = true;
            break;
        }
        // 鼠标点击选项
        if (msg.message == WM_LBUTTONDOWN) {
            for (int i = 0; i < 4; i++) {
                int oy = optStartY + i * optGap;
                if (msg.x >= optX && msg.x <= optX + optW &&
                    msg.y >= oy && msg.y <= oy + optH) {
                    answered = true;
                    correct = (i == questions[currentBombQIdx].answer);
                    break;
                }
            }
            if (answered) break;
        }
        // 键盘数字键 1~4
        if (msg.message == WM_KEYDOWN) {
            int key = -1;
            if (msg.vkcode == '1') key = 0;
            else if (msg.vkcode == '2') key = 1;
            else if (msg.vkcode == '3') key = 2;
            else if (msg.vkcode == '4') key = 3;
            if (key >= 0) {
                answered = true;
                correct = (key == questions[currentBombQIdx].answer);
                break;
            }
        }
    }
    // 如果还没做出选择，本帧结束
    if (!answered) {
        // 还原绘图状态
        setfillcolor(WHITE);
        setlinecolor(BLACK);
        settextcolor(BLACK);
        settextstyle(20, 0, _T("宋体"));
        setbkmode(OPAQUE);
        return false;
    }
    // 做出选择或取消
    isBombAnswering = false;   // 关闭答题状态

    // 还原绘图状态
    setfillcolor(WHITE);
    setlinecolor(BLACK);
    settextcolor(BLACK);
    settextstyle(20, 0, _T("宋体"));
    setbkmode(OPAQUE);

    if (cancel) {
        return false;          // 取消：不改变任何状态
    }
    if (correct) {
        // 答对：清屏、特效、开启冷却
        clearEnemies();
        bombEffectActive = true;
        bombEffectFrame = 0;
        bombEffectX = WIN_W / 2;
        bombEffectY = PATH_Y;
        bombCooldown = BOMB_COOLDOWN_FRAME;
        return true;
    }
    else {
        // 答错：本局禁用核弹
        bombAvailable = false;
        return false;
    }

}
//========================================================================








//8.23小猴子
//========================================================================
/*
 * 负责人：小猴子
 * 功能：触发核弹答题（仅有触发效果，答题逻辑在 askQuestion 内）
 *   1. 检查冷却、可用、是否已在答题
 *   2. 随机选一题，设置 currentBombQIdx，并置 isBombAnswering = true
 * 参数：无
 * 返回值：无
 */
void useBomb() {
    // 初稿：按 N 键触发，冷却结束直接清屏
    // 拓展：答题成功才清屏，答错 bombAvailable = false
    // TODO:
    // if (bombCooldown <= 0 && bombAvailable) {
    //     clearEnemies();
    //     bombCooldown = BOMB_COOLDOWN_FRAME;
    // }

    // 冷却中 / 已禁用 / 正在答题 → 不触发
    if (bombCooldown > 0 || !bombAvailable || isBombAnswering) {
        return;
    }
    // 随机抽题并进入答题状态
    currentBombQIdx = rand() % questionCount;
    isBombAnswering = true;

}
//========================================================================









//嘉豪8.23
//========================================================================
/*
 * 负责人：嘉豪
 * 功能：清除场上所有存活敌人（核弹清屏）
 *   1. 遍历 enemies 数组，统计存活敌人数量
 *   2. 将所有存活敌人标记为死亡（alive = false）
 *   3. 将统计的存活数量累加到 killedCount
 * 说明：不修改 enemyCount，保持稀疏数组设计约定
 *      （enemyCount 由 spawnEnemy 增加，由 updateEnemies 的压缩逻辑减少）
 * 参数：无
 * 返回值：无
 * 涉及全局变量：enemies[]、enemyCount、killedCount
 */
void clearEnemies() {
    // TODO: 先统计存活数量，再逐个置false
    // int add = 0;
    // for (int i=0; i<enemyCount; i++) {
    //     if (enemies[i].alive) { add++; enemies[i].alive = false; }
    // }
    // killedCount += add;

    int add = 0;
    for (int i = 0; i < enemyCount; i++) {
        Enemy* enemy = &enemies[i];
        if (enemy->alive) {
            add++;
            enemy->alive = false;
        }
    }
    killedCount += add;
}
//========================================================================













//dandan8.23
//========================================================================
/*
 * 负责人：丹丹
 * 功能：每帧判定游戏胜负
 *   1. 大本营血量<=0 → 失败结算
 *   2. 胜利条件：全部敌人已生成 + 场上无存活敌人 + 大本营血量>0
 *   3. 关卡配置非法时不做判定
 * 参数：无
 * 返回值：无
 * 涉及全局变量：baseHp、totalSpawned、enemyCount、enemies[]、
 *               currentLevel、levelConfigs、levelCount、state
 */
void checkGameStatus() {
    // 失败
    // if (baseHp <= 0) { state = STATE_DEFEAT; return; }

    // 胜利（必须同时满足）：
    // 1. totalSpawned >= levelConfigs[idx].totalEnemies
    // 2. 场上无存活敌人（遍历 enemies 检查 alive）
    // 3. baseHp > 0
    // int idx = currentLevel - 1;
    // if (idx < 0 || idx >= levelCount) return;
    //
    // bool hasAlive = false;
    // for (int i=0; i<enemyCount; i++) {
    //     if (enemies[i].alive) { hasAlive = true; break; }
    // }
    // if (totalSpawned >= levelConfigs[idx].totalEnemies &&
    //     !hasAlive && baseHp > 0) {
    //     state = STATE_VICTORY;
    // }


    // 1. 失败判定（优先级最高）
    if (baseHp <= 0) {
        state = STATE_DEFEAT;
        return;
    }
    // 2. 获取关卡配置（越界保护）
    int idx = currentLevel - 1;
    if (idx < 0 || idx >= levelCount) {
        return;   // 关卡配置非法，不做判定
    }
    // 3. 胜利条件1：所有敌人都已生成
    if (totalSpawned < levelConfigs[idx].totalEnemies) {
        return;   // 还有敌人没生成，不可能胜利
    }
    // 4. 胜利条件2：场上没有存活敌人（遍历检查）
    bool hasAlive = false;
    for (int i = 0; i < enemyCount; i++) {
        if (enemies[i].alive) {
            hasAlive = true;
            break;   // ✅ 修改点：用 break 跳出，不用 return
        }
    }
    // 5. 胜利条件3：大本营血量 > 0（失败已提前判定，这里只需检查血量）
    if (!hasAlive && baseHp > 0) {
        state = STATE_VICTORY;
    }
}
//========================================================================











// ============================================================
// 存档实现（占位）
// ============================================================

bool saveGame(int slot) {
    if (slot < 0 || slot >= SAVE_SLOT_COUNT) return false;
    // TODO: 填充SaveData，写入save_%d.dat
    // 注意：逐个成员fwrite，不要一次性写结构体
    return true;
}

bool loadGame(int slot) {
    if (slot < 0 || slot >= SAVE_SLOT_COUNT) return false;
    // TODO: 读取save_%d.dat并恢复
    return true;
}

bool hasSave(int slot) {
    if (slot < 0 || slot >= SAVE_SLOT_COUNT) return false;
    // TODO: 文件存在性检测
    return false;
}

void deleteSave(int slot) {
    if (slot < 0 || slot >= SAVE_SLOT_COUNT) return;
    // TODO: 删除文件
}

// ============================================================
// 设置实现（占位）
// ============================================================

void changeSetting(int option, int value) {
    // option: 0-音效开关, 1-音量, 2-难度, 3-音乐开关
    // 注意：音量 0~100，难度 0~2
    // TODO: 修改对应全局变量
}

// ============================================================
// 辅助函数实现（占位）
// ============================================================








//KAKA 8.22
// ============================================================
/*
负责人：卡卡
 功能：寻找当前场上最左侧的存活敌人（x最小）
   1. 遍历 enemies[0] ~ enemies[enemyCount-1]
   2. 跳过 alive == false 的敌人
   3. 记录最小 x 的敌人下标
 涉及数据：enemies[]、enemyCount、Enemy.alive、Enemy.x
 参数：无
 返回值：int
        >=0 找到的敌人数组下标（用于弓箭手索敌）
        -1   场上没有存活敌人
*/
int findLeftmostEnemy() {
    // TODO: 遍历enemies，返回x最小且alive的下标
    // int minIdx = -1;
    // int minX = 99999;
    // for (...) if (enemies[i].alive && enemies[i].x < minX) { minX = enemies[i].x; minIdx = i; }
    // return minIdx;
    int minIdx = -1;           // 最左敌人的下标，默认为-1表示没找到
    int minX = 2147483647;     // 初始化为最大整数，用于比较

    for (int i = 0; i < enemyCount; i++) {
        if (enemies[i].alive && enemies[i].x < minX) {
            minX = enemies[i].x;
            minIdx = i;
        }
    }
    return minIdx;
}
//==============================================================================








//朱涛8.21
//==============================================================================
/*
 * 负责人：朱涛
 * 功能：检测点(x,y)附近range半径范围内是否存在存活敌人
 * 参数：
 *   x, y   - 检测中心坐标
 *   range  - 检测半径（像素）
 * 返回值：
 *   true  - 范围内存在存活敌人
 *   false - 范围内无存活敌人
 * 涉及全局变量：enemies[]、enemyCount、Enemy.alive、Enemy.x、Enemy.y
 */
bool isEnemyNear(int x, int y, int range) {
    // TODO: 遍历，距离 < range
    // 遍历所有敌人
    for (int i = 0; i < enemyCount; i++) {
        // 排除死亡敌人
        if (!enemies[i].alive)
            continue;
        // 计算横纵距离差
        int dx = enemies[i].x - x;
        int dy = enemies[i].y - y;
        // 使用距离平方比较，避免开方
        if (dx * dx + dy * dy <= range * range) {
            return true;
        }
    }
    return false;
}
//==============================================================================








//卡卡8.24
//==============================================================================
/*
 * 负责人：卡卡
 * 功能：清空所有动态数组与计数
 *   1. 将所有我方单位 myUnits 标记为死亡（alive=false）
 *   2. 将所有敌人 enemies 标记为死亡
 *   3. 将所有箭矢 arrows 标记为死亡
 *   4. 重置 myUnitCount、enemyCount、arrowCount 为 0
 *   5. 重置 unitCount[] 各职业数量为 0
 * 参数：无
 * 返回值：无
 * 涉及全局变量：myUnits[]、enemies[]、arrows[]、
 *               myUnitCount、enemyCount、arrowCount、unitCount[]
 */
void clearArrays() {
    // TODO: 所有单位/敌人/箭矢 alive = false
    // myUnitCount = enemyCount = arrowCount = 0;
    // unitCount[0..4] = 0;


    // 全部我方单位数组位置标记死亡
    for (int i = 0; i < MY_UNIT_MAX; i++) {
        myUnits[i].alive = false;
    }
    myUnitCount = 0;

    for (int i = 0; i < ENEMY_MAX; i++) {
        enemies[i].alive = false;
    }
    enemyCount = 0;

    for (int i = 0; i < ARROW_MAX; i++) {
        arrows[i].alive = false;
    }
    arrowCount = 0;

    for (int i = 0; i < UT_TYPE_COUNT; i++) {
        unitCount[i] = 0;
    }
}
//==============================================================================









// ============================================================
// 主函数           fuzeren:KAKA
// ============================================================

int main() {
    initgraph(WIN_W, WIN_H);
    srand((unsigned)time(NULL));

    BeginBatchDraw();

    //main() 里写好退出条件
    while (state != STATE_EXIT) {
        switch (state) {
        case STATE_MENU:         menuView(); break;
        case STATE_GAME:         gameView(); break;
        case STATE_VICTORY:      victoryView(); break;
        case STATE_DEFEAT:       defeatView(); break;
        case STATE_PAUSE:        pauseView(); break;
        case STATE_SETTING:      settingView(); break;
        case STATE_SAVE_MANAGE:  saveManageView(); break;
        case STATE_LEVEL_SELECT: levelSelectView(); break;
        case STATE_TEAM_INTRO:   teamIntroView(); break;
        case STATE_GAME_SETTING: gameSettingView(); break;
        default: state = STATE_MENU; break;
        }

        FlushBatchDraw();
        Sleep(FRAME_MS);
    }

    // 自然退出后可执行清理代码
    EndBatchDraw();
    closegraph();
    return 0;
}