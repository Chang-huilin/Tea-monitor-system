# teaSYS

## 项目简介

**teaSYS** 是一个基于 **Qt 6 + Pleora eBUS SDK** 的工业相机上位机示例工程，支持 **GigE Vision 工业相机** 的连接、图像采集、实时显示以及手动 / 定时保存图像。

项目采用模块化设计，将 **相机 SDK 逻辑** 与 **UI 层** 解耦，适合作为工业视觉系统的基础框架或二次开发模板。

---

## 功能特性

### 相机功能

- 自动识别 GigE Vision 相机  
- 建立相机连接 
- 支持 Imperx 等 GEV 工业相机  
- GEV 自动协商网络包大小  
- 正确配置 Stream Destination  
- 连续采集（Continuous Acquisition）  
- 关闭硬触发（Trigger Off）  

### 图像采集与显示

- 单帧采集（手动拍摄）  
- 连续采集（定时器触发）  
- 支持像素格式：
  - Mono8（灰度）
  - Bayer RAW（RG / BG / GR / GB）
- Bayer → RGB 软件解码（eBUS `PvBufferConverter`）
- Qt 界面实时显示图像  
- 自动保持宽高比缩放显示  

### 图像保存

- 手动拍摄自动保存  
- 定时拍摄自动保存  
- 自动创建保存目录  
- 基于时间戳命名，避免覆盖  


### 光谱仪功能

- 自动识别海洋光学光谱仪

保存目录结构：
```text
  captures/
  ├─ manual/ 手动拍摄
  └─ timer/ 定时拍摄
```  
---

## 工程结构

```text
teaSYS/
├─ core/
│  ├─ camera_interface.h    # 相机接口定义
│  ├─ camera_service.h      # eBUS 相机实现
│  ├─ camera_service.cpp
│  │  
│  ├─ spectrometer_interface.h    # 光谱仪接口定义
│  ├─ spectrometer_service.h      # ocean 光谱仪实现
│  ├─ spectrometer_service.cpp
│
│
├─ mainwindow.h             # 主界面定义
├─ mainwindow.cpp           # UI 逻辑
├─ mainwindow.ui            # Qt Designer 文件
├─ main.cpp
├─ CMakeLists.txt
└─ README.md
```
### 模块职责说明

| 模块 | 说明 |
|----|----|
| `CameraService` | 相机连接、采集、像素格式转换 |
| `MainWindow` | UI、按钮、定时器、图像显示、文件保存 |
| `core/` | 与 eBUS SDK 强相关的底层实现 |

设计原则：

- `CameraService` **不负责文件保存**
- `MainWindow` **不直接调用 eBUS SDK**
- 模块之间通过 `QImage` 传递图像数据

---

## 编译环境

### 必需组件

- Windows 10 / 11（64-bit）
- Qt 6.x（MSVC 2022 64-bit）
- CMake ≥ 3.16
- Pleora **eBUS SDK（Windows x64）**
- 支持 GigE Vision 的工业相机

### 已验证环境

- Qt 6.10.1  
- MSVC 2022  
- Pleora eBUS SDK（GEV）  
- Imperx GEV 系列相机  

---

## CMake 配置要点

在 `CMakeLists.txt` 中配置 eBUS SDK 路径：

```cmake
set(EBUS_ROOT "D:/SDK/eBUS SDK")

set(EBUS_INCLUDE_DIR "${EBUS_ROOT}/Includes")
set(EBUS_LIB_DIR     "${EBUS_ROOT}/Libraries")
```
### 链接必要库：
```cmake
target_include_directories(teaSYS PRIVATE ${EBUS_INCLUDE_DIR})
target_link_directories(teaSYS PRIVATE ${EBUS_LIB_DIR})
target_link_libraries(teaSYS PRIVATE
    Qt6::Widgets
    Qt6::Gui
    PvBase
)
```
## 使用说明

1. **启动程序**  
   运行 `teaSYS.exe`

2. **连接相机**  
   - 点击 “连接相机”  
   - 程序会自动枚举相机  
   - 默认连接第一台可用设备  
   - 状态栏显示相机型号  

3. **手动拍摄**  
   - 点击 “手动拍摄”  
   - 立即采集一帧  
   - 显示到界面  
   - 自动保存到 `captures/manual/`  

4. **定时拍摄**  
   - 点击 “开始定时抓拍”  
   - 按固定时间间隔采集  
   - 显示并保存到 `captures/timer/`
