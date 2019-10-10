#!/bin/bash
#!/bin/sh

# proto编译
protoc --cpp_out=. ./*.proto


# 将全部的cc 文件 变成 cpp文件
oldsuffix="cc"
newsuffix="cpp"
dir=$(eval pwd)
for file in $(ls $dir | grep .${oldsuffix})
do
    name=$(ls ${file} | cut -d. -f1,2)
    mv $file ${name}.${newsuffix}
done
echo "build proto file successd!"

