# Unit 迭代器

为 `Unit` 类提供只读迭代器，支持高效遍历活跃子单元。

## 功能特性

- **只读迭代器**: 仅提供 `const_iterator`，保证线程安全
- **高效遍历**: 通过 `active_index_cache` 直接访问有效子单元，跳过已标记删除的子单元
- **锁策略**: 构造时获取共享锁，复制索引后立即释放，遍历过程无需加锁

## 使用方法

### 基本遍历

```cpp
for (const auto& child : unit->begin(), unit->end()) {
    if (child != nullptr) {
        // 处理子单元
    }
}

// 简化写法 (C++11 range-for)
for (const auto& child : *unit) {
    // child 类型为 const Unit*
}
```

### 获取子单元数量

```cpp
auto count = unit->get_children_count();
```

### 使用迭代器

```cpp
auto it = unit->begin();
auto end = unit->end();

for (; it != end; ++it) {
    const Unit* child = *it;
    // 处理 child
}
```

### 结合算法

```cpp
#include <algorithm>

auto it = std::find_if(unit->begin(), unit->end(), 
    [](const Unit* child) {
        return child && child->name == "target";
    });
```

## 运算符支持

| 运算符 | 描述 |
|--------|------|
| `*` | 解引用，返回 `const Unit*` |
| `->` | 箭头访问 |
| `++` | 前置递增 |
| `--` | 前置递减 |
| `==` | 相等比较 |
| `!=` | 不相等比较 |

## 线程安全

- 迭代器构造时获取 `std::shared_lock` 读取 `active_index_cache`
- 索引数据被复制到迭代器内部，锁立即释放
- 遍历过程中无需持有锁，安全且高效

## 注意事项

- 迭代器是只读的，不支持修改底层数据
- 迭代器遍历的是**活跃子单元**（未标记删除的子单元）
- `end()` 返回的迭代器解引用时返回 `nullptr`
