#include "../pch.h"
#include "FileMemory.h"

FileMemory::FileMemory() : rawdata(nullptr), datasize(0), in(data), out(data)
{
    //4����Ʈ ������, ��id, ������
    //�� �Է� ������ x -> z -> y ������ y -> z -> x
}