TARGET 		= SmartCalc_2.0

TEST_TARGET = Test_SmartCalc_2.0

DIR			= SmartCalc

TEST_DIR	= $(DIR)/source

TEST_OBJ_DIR = $(DIR)/test_obj

TEST_SOURCE = \
		test_model_calc.cc \
		model_calc.cc \
		test_model_finance.cc \
		model_finance.cc \

HEADER 		= \
		$(DIR)/include/model_calc.h \
		$(DIR)/include/model_finance.h \

TEST_OBJ	= $(TEST_SOURCE:%.cc=$(TEST_OBJ_DIR)/%.o)

DOC_INFO 	= doc.info

RM 			= rm -rf

CC			= g++

CFLAGS 			= 	-std=c++17 -Wall -Wextra -Werror -I$(DIR)/include
LDFLAGS 		= 	-lgtest 

.PHONY 	: all install uninstall clean dvi dist tests

all 	: install tests
		@./$(TEST_TARGET)
		@$(MAKE) code_style

install :
		@cd $(DIR) && qmake6 "TARGET=$(TARGET)" SmartCalc.pro
		@$(MAKE) -C ./$(DIR)

clean 	:
		@if ([ ! -d $(TEST_OBJ_DIR) ] && [ ! -d $(DIR)/obj ] && [ ! -d $(DIR)/moc ] && \
		[ ! -f $(DIR)/.qmake.stash ] && [ ! -f SmartCalc/ui_mainwindow.h ]); then \
			echo "make: Nothing to be done for 'make clean'."; fi;
		@if [ -d $(TEST_OBJ_DIR) ]; then $(RM) $(TEST_OBJ_DIR); \
			echo "Removed $(TEST_OBJ_DIR) (directory) successfully!"; fi;
		@if [ -d $(DIR)/obj ]; then $(RM) $(DIR)/obj; \
			echo "Removed $(DIR)/obj (directory) successfully!"; fi;
		@if [ -d $(DIR)/moc ]; then $(RM) $(DIR)/moc; \
			echo "Removed $(DIR)/moc (directory) successfully!"; fi;
		@if [ -f $(DIR)/.qmake.stash ]; then $(RM) $(DIR)/.qmake.stash; \
			echo "Removed $(DIR)/.qmake.stash (file) successfully!"; fi;
		@if [ -f $(DIR)/ui_mainwindow.h ]; then $(RM) $(DIR)/ui_mainwindow.h; \
			echo "Removed $(DIR)/ui_mainwindow.h (file) successfully!"; fi;

uninstall :
		@if ([ ! -f $(DIR)/Makefile ] && [ ! -f $(TARGET) ] && [ ! -f $(TEST_TARGET) ] && \
		[ ! -d $(TEST_OBJ_DIR) ] && [ ! -d $(DIR)/obj ] && [ ! -d $(DIR)/moc ] &&  [ ! -f $(DOC_INFO) ] && \
		[ ! -f $(DIR)/.qmake.stash ] && [ ! -f $(DIR)/ui_mainwindow.h ]); then \
			echo "make: Nothing to be done for 'make uninstall'."; fi;
		@if [ -f $(DIR)/Makefile ]; then $(RM) $(DIR)/Makefile; \
			echo "Removed $(DIR)/Makefile (file) successfully!"; fi;
		@if [ -f $(TARGET) ]; then $(RM) $(TARGET); \
			echo "Removed $(TARGET) (file) successfully!"; fi;
		@if [ -f $(TEST_TARGET) ]; then $(RM) $(TEST_TARGET); \
			echo "Removed $(TEST_TARGET) (file) successfully!"; fi;
		@if [ -d $(TEST_OBJ_DIR) ]; then $(RM) $(TEST_OBJ_DIR); \
			echo "Removed $(TEST_OBJ_DIR) (directory) successfully!"; fi;
		@if [ -d $(DIR)/obj ]; then $(RM) $(DIR)/obj; \
			echo "Removed $(DIR)/obj (directory) successfully!"; fi;
		@if [ -d $(DIR)/moc ]; then $(RM) $(DIR)/moc; \
			echo "Removed $(DIR)/moc (directory) successfully!"; fi;
		@if [ -f $(DOC_INFO) ]; then $(RM) $(DOC_INFO); \
			echo "Removed $(DOC_INFO) (file) successfully!"; fi;
		@if [ -f $(DIR)/.qmake.stash ]; then $(RM) $(DIR)/.qmake.stash; \
			echo "Removed $(DIR)/.qmake.stash (file) successfully!"; fi;
		@if [ -f $(DIR)/ui_mainwindow.h ]; then $(RM) $(DIR)/ui_mainwindow.h; \
			echo "Removed $(DIR)/ui_mainwindow.h (file) successfully!"; fi;

dvi: $(DOC_INFO)

$(DOC_INFO) :  SmartCalc/source/doc.texi
		makeinfo $< -o doc.info

dist 	:
		$(shell mkdir SmartCalc-2.0)
		$(shell cp -r SmartCalc SmartCalc-2.0)
		$(shell cp Makefile SmartCalc-2.0)
		@$(MAKE) -C SmartCalc-2.0 uninstall
		$(shell tar -czf SmartCalc-2.0.tar SmartCalc-2.0)
		@$(RM) SmartCalc-2.0

$(TEST_TARGET) : $(TEST_OBJ)
		@$(CC) $(CFLAGS) $(TEST_OBJ) $(LDFLAGS) -o $(TEST_TARGET)
		@echo "Compiled $(TEST_TARGET) successfully!"

$(TEST_OBJ) : $(HEADER)
$(TEST_OBJ_DIR)/%.o : $(TEST_DIR)/%.cc
		@mkdir -p $(TEST_OBJ_DIR)
		@$(CC) $(CFLAGS) -c $< -o $@
		@echo "Compiled "$@" successfully!"

tests : $(TEST_TARGET)

code_style :
	clang-format -n --style=Google $(DIR)/include/*.h $(DIR)/source/*.cc
