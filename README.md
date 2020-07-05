## 想做个动态壁纸

对于 Windows Terminal (Preview) 的配置有背景图设置:

```json

                "backgroundImage" : "%GITHUB_PATH%\\Pictures\\Gif\\GhostintheShell.gif",
                "backgroundImageOpacity" : 0.3,
                "backgroundImageAlignment" : "bottomRight",
                "backgroundImageStretchMode" : "none"
```

![img](https://raw.githubusercontent.com/Ron2014/Ron2014.github.io/master/assets/images/livingwallpaper.gif)

对. 它支持动图.

左边文字, 右边图片, 有点电影谢幕的feel.

我给每个系统都配了背景图, 但是还是不太满足.

现在想要这样的功能, 指定一些存放图片的路径, 每次打开一个 panel 会读取不同的背景图 (格式为gif/jpg/png)

保证每个 panel 的背景图是不一样的, 以免切换窗口时会认错.

## 分析

只用关注 backgroundImage 这一项即可.

backgroundImage 支持环境变量读取.

但是我们又不能把所有 panel 的设置都指向一个环境变量.

直接修改文件也是可行的, lua-cjson, 但是这种读入文件转格式, 又要转回来的话, 会破坏注释的内容. 不太推荐

还是老老实实列10个环境变量来做这种事情吧.

## 环境变量

### 小问题

`_findfirst` 无法识别字符串中环境变量 `%GITHUB_PATH%` 的值, 所以我避免使用了环境变量.

### 大问题

**子进程无法更改父进程的环境变量.** 所有程序都是通过操作系统启动的, 我试了3种API, 均无法实现.

得出结论, 只有两条路可以选择:

1. 修改Windows注册表
2. 还是修改json文件方便些啊 >_<

### 自己的选择

老老实实的我, 还是不想用json做(字符串处理太无聊), 使用注册表API试了一下, 果然可以完成这些功能.

所以对这个工程, 你只要

1. cmake --build . --config Release
2. 创建一个文件夹, 将 WallPaper.exe config.ini run.bat 扔过去
3. WallPaper.exe 一定要在属性中设置 `run as administrator` (因为要改注册表)
4. Windows Terminal 的 settings.json 配置 backgroundImage 改成 config.ini [NAME] 对应的值
5. 然后起个 Windows 计划任务, 每分钟去跑 run.bat. 例如

```batch
schtasks /create /tn Living Wallpaper For Terminal /tr run.bat /sc minute /mo 1
```

### 修改注册表, exe 就需要 admin 权限

这个我交给了 setup.bat 完成.

## Windows的窗口隐藏(后台运行)

bat 的 start /B 没用

cmd 的 没用

```
if "%1" == "h" goto begin 
mshta vbscript:createobject("wscript.shell").run("%~nx0 h",0)(window.close)&&exit 
:begin 
```

vsb 的 没用

```
DIM objShell 
set objShell = wscript.createObject("wscript.shell") 
iReturn = objShell.Run("cmd.exe /C run.bat", 0, TRUE)
```

CreateProcess 会有 crash

只有两个方法最有效:

1. shellexecute : 子进程是无窗口了, 但是父窗口还是会显示啊!
2. 最简单的方法:

```c
	HWND hwnd = GetForegroundWindow();
	ShowWindow(hwnd, false);
```

只是这个方法有个BUG, 程序触发时千万别切换窗口, 不然你的窗口就真找不到了.

当然这个方法明显的弊端是 GetForegroundWindow. 我将这个接口改成搜索自己的窗口句柄了.

## 文件的移动

这个工程涉及的静态文件太多, 我全部都放到 config/ 里面了.

为了编译工程时能自动移动文件, 我添加了 CMakeFiles.txt 中的 add_custom_target 部分.

## 操作说明

1. mkdir build
2. cd build
3. cmake ..
4. cmake --build . --config release

然后资源管理器进入到 Release 目录

对 setup.bat 鼠标右键使用 "Run As Administrator".

setup.bat 做了两件事

1. 赋予 WallPaper.exe 'Run As Administrator' 权限.
2. 添加计划任务.

环境变量有了, 这时候, 可以打开 Windows Termianal, 菜单栏/设置(`CTRL+,`) 打开 settings.json

将 backgroundImage 逐个设置为 config.ini 中 [NAME] 对应的值

## 一些絮叨

需要注意的是, 一个 Windows Terminal 进程 其实是操作系统进程的子进程, 环境变量在子进程中有缓存.

如果你更新了 data.ini 想立即看到效果, 请新开一个 Windows Terminal, 而不是仅打开一个 Tab 页.