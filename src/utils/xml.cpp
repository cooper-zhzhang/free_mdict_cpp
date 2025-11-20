#include "include/utils/xml.h"
#include <map>
#include <iostream>

/*<Library_Data GeneratedByEngineVersion="2.0"
 RequiredEngineVersion="2.0"
  Encrypted="No"
   Encoding=""
    Format=""
     CreationDate="2025-1-9"
      KeyCaseSensitive="No"
       Stripkey="No"
        Description=""
         Title=""
          RegisterBy=""
           />
           */

namespace FreeMDict {
namespace Utils {

    std::map<std::string, std::string> parseXmlHead(const std::string &xml_str){
            // 存储解析出的属性键值对
    std::map<std::string, std::string> attributes;
    
    // 跳过开头的空白字符
    size_t pos = 0;
    while (pos < xml_str.size() && std::isspace(xml_str[pos])) {
        pos++;
    }
    
    // 合并为一个循环：同时处理空格分割和等号解析
    while (pos < xml_str.size()) {
        // 跳过空白字符
        while (pos < xml_str.size() && std::isspace(xml_str[pos])) {
            pos++;
        }
        
        if (pos >= xml_str.size() || xml_str[pos] == '/' || xml_str[pos] == '>') {
            break; // 到达标签结尾
        }
        
        // 解析键名（从当前位置到等号或空格）
        size_t key_start = pos;
        while (pos < xml_str.size() && xml_str[pos] != '=' && !std::isspace(xml_str[pos])) {
            pos++;
        }
        
        // 提取键名
        std::string key(xml_str, key_start, pos - key_start);
        
        // 跳过键名后的空白字符
        while (pos < xml_str.size() && std::isspace(xml_str[pos])) {
            pos++;
        }
        
        std::string value;
        // 检查是否有等号
        if (pos < xml_str.size() && xml_str[pos] == '=') {
            pos++; // 跳过等号
            
            // 跳过等号后的空白字符
            while (pos < xml_str.size() && std::isspace(xml_str[pos])) {
                pos++;
            }
            
            // 解析值（支持带引号和不带引号的情况）
            if (pos < xml_str.size() && (xml_str[pos] == '"' || xml_str[pos] == '\'')) {
                char quote = xml_str[pos];
                pos++; // 跳过开始引号
                size_t value_start = pos;
                
                // 查找结束引号
                while (pos < xml_str.size() && xml_str[pos] != quote) {
                    pos++;
                }
                
                // 提取引号内的值
                value = std::string(xml_str, value_start, pos - value_start);
                if (pos < xml_str.size()) {
                    pos++; // 跳过结束引号
                }
            } else {
                // 不带引号的值，直到遇到空格或标签结束
                size_t value_start = pos;
                while (pos < xml_str.size() && !std::isspace(xml_str[pos]) && xml_str[pos] != '/' && xml_str[pos] != '>') {
                    pos++;
                }
                
                // 提取不带引号的值
                value = std::string(xml_str, value_start, pos - value_start);
            }
        }
        
        // 存储键值对
        attributes[key] = value;
    }
    
    return attributes;
    }

std::map<std::u16string, std::u16string> parseXmlHead(const std::u16string &head){
    // 存储解析出的属性键值对
    std::map<std::u16string, std::u16string> attributes;
    
    // 跳过开头的空白字符
    size_t pos = 0;
    while (pos < head.size() && std::isspace(head[pos])) {
        pos++;
    }
    
    // 合并为一个循环：同时处理空格分割和等号解析
    while (pos < head.size()) {
        // 跳过空白字符
        while (pos < head.size() && std::isspace(head[pos])) {
            pos++;
        }
        
        if (pos >= head.size() || head[pos] == u'/' || head[pos] == u'>') {
            break; // 到达标签结尾
        }
        
        // 解析键名（从当前位置到等号或空格）
        size_t key_start = pos;
        while (pos < head.size() && head[pos] != u'=' && !std::isspace(head[pos])) {
            pos++;
        }
        
        // 提取键名
        std::u16string key(head, key_start, pos - key_start);
        
        // 跳过键名后的空白字符
        while (pos < head.size() && std::isspace(head[pos])) {
            pos++;
        }
        
        std::u16string value;
        // 检查是否有等号
        if (pos < head.size() && head[pos] == u'=') {
            pos++; // 跳过等号
            
            // 跳过等号后的空白字符
            while (pos < head.size() && std::isspace(head[pos])) {
                pos++;
            }
            
            // 解析值（支持带引号和不带引号的情况）
            if (pos < head.size() && (head[pos] == u'"' || head[pos] == u'\'')) {
                char quote = head[pos];
                pos++; // 跳过开始引号
                size_t value_start = pos;
                
                // 查找结束引号
                while (pos < head.size() && head[pos] != quote) {
                    pos++;
                }
                
                // 提取引号内的值
                value = std::u16string(head, value_start, pos - value_start);
                if (pos < head.size()) {
                    pos++; // 跳过结束引号
                }
            } else {
                // 不带引号的值，直到遇到空格或标签结束
                size_t value_start = pos;
                while (pos < head.size() && !std::isspace(head[pos]) && head[pos] != u'/' && head[pos] != u'>') {
                    pos++;
                }
                
                // 提取不带引号的值
                value = std::u16string(head, value_start, pos - value_start);
            }
        }
        
        // 存储键值对
        attributes[key] = value;
    }
    
    return attributes;
}

} // namespace Utils
} // namespace FreeMDict
