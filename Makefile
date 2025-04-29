# =====================  编译器与通用参数  =====================
CXX      := g++
CXXFLAGS := -std=c++17 -Wall -O2 -Iinclude \
            -MMD -MP                   # 生成 .d 依赖文件

# =====================  源文件收集  ==========================
SRCS_TOP := $(wildcard *.cpp)                 # 根目录
SRCS_DIR := $(shell find src -name '*.cpp')   # 所有 src/ 下
SRCS     := $(SRCS_TOP) $(SRCS_DIR)

# =====================  目标文件与目录保持  ==================
BUILDDIR := build
OBJS     := $(addprefix $(BUILDDIR)/,$(SRCS:.cpp=.o))
DEPS     := $(OBJS:.o=.d)                     # 自动依赖 .d

# =====================  输出可执行文件  ======================
TARGET   := $(BUILDDIR)/compiler

# =====================  默认目标  ===========================
.PHONY: all
all: $(TARGET)

# ------------------ 链接 ------------------
$(TARGET): $(OBJS)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $^ -o $@

# ------------------ 编译 ------------------
# %% 把 src/grammar/foo.cpp → build/src/grammar/foo.o
$(BUILDDIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# ------------------ 运行（可选）------------
.PHONY: run
run: $(TARGET)
	@$(TARGET) $(FILE)

# ------------------ 清理 -------------------
.PHONY: clean
clean:
	rm -rf $(BUILDDIR)

# =====================  自动包含依赖  ========================
# 仅当依赖文件存在时才 include
-include $(DEPS)
