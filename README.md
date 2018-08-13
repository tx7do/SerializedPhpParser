# Php Serialized Parser

# HOW TO USE：
```cpp
auto str = "a:2:{s:4:\"tool\";s:15:\"php unserialize\";s:6:\"author\";s:13:\"1024tools.com\";}";
SerializedPhpParser parser(str);
auto arrItem = parser.parse();
auto strTool = arrItem["tool"].asCString();
auto strAuthor = arrItem["author"].asCString();
printf("tool:[%s] author:[%s]\n", strTool, strAuthor);
```
