# Copyright 2024 team-d2

# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at

#     http://www.apache.org/licenses/LICENSE-2.0

# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# cmakeの設定
cmake_minimum_required(VERSION 3.8)
project(action5_ros)

# 必要なパッケージのinclude
find_package(ament_cmake_auto REQUIRED)
ament_auto_find_build_dependencies()
find_package(OpenCV REQUIRED)

# c++ファイルのビルド
file(GLOB_RECURSE "${PROJECT_NAME}_lib_files"
  LIST_DIRECTORIES false
  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
  "src/*/*")
list(LENGTH ${PROJECT_NAME}_lib_files ${PROJECT_NAME}_lib_files_count)
if(${PROJECT_NAME}_lib_files_count GREATER 0)
  ament_auto_add_library("${PROJECT_NAME}_lib" SHARED "${${PROJECT_NAME}_lib_files}")
  target_compile_features("${PROJECT_NAME}_lib" PUBLIC c_std_99 cxx_std_17)
  if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(-O2 -Wall -Wextra -Wpedantic)
  endif()
  target_link_libraries("${PROJECT_NAME}_lib" "${cpp_typesupport_target}" ${OpenCV_LIBS})
endif()

# ノードプラグインの設定
rclcpp_components_register_node("${PROJECT_NAME}_lib"
  PLUGIN "${PROJECT_NAME}::Action5Node"
  EXECUTABLE action5_node)

# ディレクトリのinstall
set(install_directories config;launch)
foreach(install_directory IN LISTS install_directories)
  if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${install_directory}")
    install(DIRECTORY
      "${install_directory}"
      DESTINATION share/${PROJECT_NAME}/)
  endif()
endforeach()

# テスト
if(BUILD_TESTING)
  ament_auto_find_test_dependencies()
  ament_lint_auto_find_test_dependencies()
  # file(GLOB_RECURSE "${PROJECT_NAME}_test_src_files"
  #   LIST_DIRECTORIES false
  #   RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
  #   "test/*.cpp")
  # ament_auto_add_gtest(${PROJECT_NAME}_test
  #   "${${PROJECT_NAME}_test_src_files}"
  #   TIMEOUT "30")
  # target_compile_features("${PROJECT_NAME}_test" PUBLIC c_std_99 cxx_std_17)
  # target_link_libraries("${PROJECT_NAME}_test" "${cpp_typesupport_target}" fmt)
endif()

# パッケージング
ament_auto_package()
