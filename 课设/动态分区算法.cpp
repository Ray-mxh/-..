#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
// 内存管理系统常量定义
#define MAX_FREE_ENTRIES 20  // 最大空闲分区表项数
#define MAX_ALLOC_ENTRIES 20  // 最大已分配分区表项数
// 内存分区数据结构 - 表示内存中的一个连续区域
struct map {
    int m_addr;     // 分区起始地址（内存物理地址）
    int m_size;     // 分区大小（以KB为单位）
};

// 最先适应(First-Fit)算法
// 遍历空闲分区表，找到第一个满足大小要求的分区进行分配
int first_fit(struct map *free_list, int free_count, int size, struct map *allocated) {
    int i;
    for (i = 0; i < free_count; i++) {
        // 找到第一个足够大的空闲分区
        if (free_list[i].m_size >= size) {
            // 分配该分区给请求
            allocated->m_addr = free_list[i].m_addr;
            allocated->m_size = size;
            // 更新空闲分区表 - 分割剩余空间
            free_list[i].m_addr += size;  // 起始地址后移
            free_list[i].m_size -= size;  // 剩余大小减少
            // 如果分割后剩余空间为0，移除该表项
            if (free_list[i].m_size == 0) {
                for (int j = i; j < free_count - 1; j++) {
                    free_list[j] = free_list[j + 1];  // 前移覆盖
                }
                free_count--;  // 空闲分区数量减1
            }
            return allocated->m_addr;  // 返回分配的起始地址
        }
    }
    return -1;  // 分配失败，没有足够空间
}
// 按分区大小升序排序 - 用于最佳适应算法
// 冒泡排序实现，将最小的分区排在前面
void sort_by_size_asc(struct map *list, int count) {
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (list[j].m_size > list[j + 1].m_size) {
                struct map temp = list[j];
                list[j] = list[j + 1];
                list[j + 1] = temp;
            }
        }
    }
}
// 最佳适应(Best-Fit)算法
// 找到能满足需求的最小空闲分区，减少内部碎片
int best_fit(struct map *free_list, int free_count, int size, struct map *allocated) {
    int best_idx = -1;  // 最佳分区索引
    int min_diff = INT_MAX;  // 记录最佳分区与需求的差值（最小剩余空间）
    // 查找最合适的分区
    for (int i = 0; i < free_count; i++) {
        if (free_list[i].m_size >= size) {
            int diff = free_list[i].m_size - size;  // 计算剩余空间
            if (diff < min_diff) {  // 找到更小的剩余空间
                min_diff = diff;
                best_idx = i;
            }
        }
    }
    // 未找到合适分区
    if (best_idx == -1) return -1;
    
    // 分配并分割分区
    allocated->m_addr = free_list[best_idx].m_addr;
    allocated->m_size = size;
    free_list[best_idx].m_addr += size;
    free_list[best_idx].m_size -= size;
    // 处理空分区
    if (free_list[best_idx].m_size == 0) {
        for (int j = best_idx; j < free_count - 1; j++) {
            free_list[j] = free_list[j + 1];
        }
        free_count--;
    }
    return allocated->m_addr;
}
// 按分区大小降序排序 - 用于最坏适应算法
// 冒泡排序实现，将最大的分区排在前面
void sort_by_size_desc(struct map *list, int count) {
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (list[j].m_size < list[j + 1].m_size) {
                struct map temp = list[j];
                list[j] = list[j + 1];
                list[j + 1] = temp;
            }
        }
    }
}
// 最坏适应(Worst-Fit)算法
// 分配最大的可用分区，使剩余分区仍能满足较大需求
int worst_fit(struct map *free_list, int free_count, int size, struct map *allocated) {
    int worst_idx = -1;  // 最大分区索引
    
    // 查找最大的合适分区（已按降序排列，第一个即为最大）
    for (int i = 0; i < free_count; i++) {
        if (free_list[i].m_size >= size) {
            worst_idx = i;
            break;
        }
    }  
    // 未找到合适分区
    if (worst_idx == -1) return -1;
    // 分配并分割分区
    allocated->m_addr = free_list[worst_idx].m_addr;
    allocated->m_size = size;
    free_list[worst_idx].m_addr += size;
    free_list[worst_idx].m_size -= size;
    // 处理空分区
    if (free_list[worst_idx].m_size == 0) {
        for (int j = worst_idx; j < free_count - 1; j++) {
            free_list[j] = free_list[j + 1];
        }
        free_count--;
    }
    return allocated->m_addr;
}
// 内存释放函数，处理分区合并
// 合并相邻的空闲分区，减少外部碎片
int free_memory(struct map *free_list, int *free_count, struct map allocated) {
    int i, j;
    int merged = 0;  // 标记是否发生合并
    struct map new_free = allocated;  // 待释放的分区
    // 检查是否与前一个分区邻接
    for (i = 0; i < *free_count; i++) {
        if (free_list[i].m_addr + free_list[i].m_size == new_free.m_addr) {
            // 合并前一个分区（当前分区末尾与新分区起始相邻）
            free_list[i].m_size += new_free.m_size;
            new_free = free_list[i];  // 更新合并后的分区
            free_list[i] = free_list[*free_count - 1];  // 移除最后一个元素
            (*free_count)--;
            merged = 1;
            break;
        }
    }
    
    // 检查是否与后一个分区邻接
    for (i = 0; i < *free_count; i++) {
        if (new_free.m_addr + new_free.m_size == free_list[i].m_addr) {
            // 合并后一个分区（新分区末尾与当前分区起始相邻）
            free_list[i].m_addr = new_free.m_addr;
            free_list[i].m_size += new_free.m_size;
            merged = 1;
            break;
        }
    }
    // 若未合并，插入新空闲分区
    if (!merged) {
        if (*free_count >= MAX_FREE_ENTRIES) return 0;  // 超出表大小
        free_list[*free_count] = new_free;
        (*free_count)++;
        // 按地址排序空闲区表（适用于最先适应）
        for (i = 0; i < *free_count - 1; i++) {
            for (j = 0; j < *free_count - i - 1; j++) {
                if (free_list[j].m_addr > free_list[j + 1].m_addr) {
                    struct map temp = free_list[j];
                    free_list[j] = free_list[j + 1];
                    free_list[j + 1] = temp;
                }
            }
        }
    }
    return 1;  // 释放成功
}
// 显示内存状态 - 展示空闲和已分配分区信息
void display_memory(struct map *free_list, int free_count, struct map *allocated_list, int alloc_count) {
    printf("\n===== 内存状态 =====");
    printf("\n--- 空闲分区 ---");
    if (free_count == 0) printf("\n无空闲分区");
    for (int i = 0; i < free_count; i++) {
        printf("\n%d: 起始地址=%dKB, 大小=%dKB", i, free_list[i].m_addr, free_list[i].m_size);
    }
    printf("\n--- 已分配分区 ---");
    if (alloc_count == 0) printf("\n无已分配分区");
    for (int i = 0; i < alloc_count; i++) {
        printf("\n%d: 起始地址=%dKB, 大小=%dKB", i, allocated_list[i].m_addr, allocated_list[i].m_size);
    }
    printf("\n===================\n");
}
int main() {
    struct map free_list[MAX_FREE_ENTRIES];  // 空闲分区表
    struct map allocated_list[MAX_ALLOC_ENTRIES];  // 已分配分区表
    int free_count = 0;  // 空闲分区数量
    int alloc_count = 0;  // 已分配分区数量
    int choice, size, addr, algo_choice;  // 用户选择、分配大小、分配地址、算法选择
    // 初始化：假设总内存从0开始，大小为1024KB
    free_list[0].m_addr = 0;
    free_list[0].m_size = 1024;
    free_count = 1;
    printf("动态分区存储管理实验\n");
    printf("1. 最先适应算法(FF) - 实现简单，速度快\n");
    printf("2. 最佳适应算法(BF) - 减少内部碎片\n");
    printf("3. 最坏适应算法(WF) - 保留大分区\n");
    printf("请选择算法: ");
    scanf("%d", &algo_choice);
    while (1) {
        printf("\n操作选项:\n");
        printf("1. 分配内存 - 请求指定大小的内存分区\n");
        printf("2. 释放内存 - 归还已分配的内存分区\n");
        printf("3. 显示内存状态 - 查看当前内存使用情况\n");
        printf("4. 退出 - 结束内存管理模拟\n");
        printf("请选择: ");
        scanf("%d", &choice);
        switch (choice) {
            case 1:
                printf("输入分配大小(KB): ");
                scanf("%d", &size);
                if (size <= 0) {
                    printf("分配大小必须为正数\n");
                    break;
                } 
                // 根据用户选择的算法执行内存分配
                if (algo_choice == 1) {
                    addr = first_fit(free_list, free_count, size, &allocated_list[alloc_count]);
                } else if (algo_choice == 2) {
                    sort_by_size_asc(free_list, free_count);  // 排序以找到最佳分区
                    addr = best_fit(free_list, free_count, size, &allocated_list[alloc_count]);
                } else {
                    sort_by_size_desc(free_list, free_count);  // 排序以找到最大分区
                    addr = worst_fit(free_list, free_count, size, &allocated_list[alloc_count]);
                }
                if (addr != -1) {
                    printf("分配成功，起始地址: %dKB\n", addr);
                    alloc_count++;  // 更新已分配分区数量
                } else {
                    printf("分配失败，内存不足\n");
                }
                break;
            case 2:
                printf("输入已分配分区索引: ");
                int idx;
                scanf("%d", &idx);
                if (idx >= 0 && idx < alloc_count) {
                    if (free_memory(free_list, &free_count, allocated_list[idx])) {
                        // 移除已分配表项
                        for (int i = idx; i < alloc_count - 1; i++) {
                            allocated_list[i] = allocated_list[i + 1];
                        }
                        alloc_count--;
                        printf("释放成功\n");
                    } else {
                        printf("释放失败，空闲表已满\n");
                    }
                } else {
                    printf("无效的分区索引\n");
                }
                break;
            case 3:
                display_memory(free_list, free_count, allocated_list, alloc_count);
                break;
            case 4:
                printf("程序退出\n");
                return 0;
                
            default:
                printf("无效选择，请重新输入\n");
        }
    }
    return 0;
}
