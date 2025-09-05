## API接口

| 功能描述           | URI 路径                    | 请求方式 | 请求内容类型       | 说明                             |
| ------------------ | --------------------------- | -------- | ------------------ | -------------------------------- |
| 获取设备信息       | `/api/device/info`          | GET      | N/A                | 返回网络信息、系统时间、CSAC状态 |
| 同步设备时间       | `/api/device/sync_time`     | GET      | N/A                | 后端立即同步系统时间             |
| 查询是否能启动驯服 | `/api/discipline/can_start` | GET      | N/A                | 返回是否满足训练启动条件         |
| 启动驯服           | `/api/discipline/start`     | POST     | `application/json` | 启动 GPS 驯服                    |
| 查询驯服状态       | `/api/discipline/status`    | GET      | N/A                | 返回训练实时状态信息             |

##   获取设备信息

后端接收请求，获取：

- 网络信息：IP，MAC
- 时间信息：date
- 铷钟信息：status、temp等

### 铷钟接口

获取铷钟信息的详细接口在`ruclock/include/telemetry_query.h`中

```C
int get_telemetry_data(T_CSAC_telemetry *telemetry);
```

### 响应格式

```json
{
  "network": {
    "ip": "192.168.1.10",
    "mac": "AA:BB:CC:DD:EE:FF"
  },
  "system": {
    "date": "2025-06-19 11:20:00"
  },
  "csac": {
    "status": 0x0000,
    "temp": 41.5,
    "steer": 12,
    "phase": 33
  }
}
```

## 同步设备时间

后端接受请求后，同步设备时间与GPS信号一致。

### 请求

```http
POST /api/device/sync_time
Host: 10.1.2.140:5000
```

### 响应

```json
{ "success": true, "message": "时间同步成功" }
```

## 查询是否能启动驯服

### 铷钟接口

通过调用铷钟模块`ruclock/include/discipliner.h`

```C
/**
 * @brief 检查是否可以进行驯服
 * 满足的条件：
 *   1. 铷钟状态稳定
 *   2. 存在GPS 1PPS输入 
 * @return true = 可以 ，false = 不可以
 */
bool discipliner_is_ready_to_execute();
```

### 请求

```http
GET /api/discipline/can_start
Host: 10.1.2.140:5000
```

### 响应

```json
{
  "can_start": true,
  "reason": "条件满足"
}
```

## 启动驯服

输入训练时间常数和相位阈值，进行GPS驯服铷钟流程

### 铷钟接口

调用铷钟模块`ruclock/include/ruclock.h`的

```C
DisciplineStartResult ruclock_discipliner_start_training(uint8_t ns_threshold, uint16_t time_constant);
```

### 请求

```http
POST /api/discipline/start
Host: 10.1.2.140:5000
Content-Type: application/json

{
  "time_constant": 900,
  "ns_threshold": 1000
}
```

### 响应

```json
{
  "success": true,
  "code": 0,
  "message": "驯服开始成功"
}
```

##  查询驯服状态

### 铷钟接口

调用铷钟模块`ruclock/include/ruclock.h`

```C
DiscipliningStatus ruclock_discipliner_update_training_status();
```

### 请求

```http
GET /api/discipline/status
Host: 10.1.2.140:5000
```

### 响应

```json
{
  "disOK": 0,
  "phase": 45,
  "steer": 12,
  "training_status": 0,
  "message": "训练中：Steer=12, Phase=45 ns"
}
```