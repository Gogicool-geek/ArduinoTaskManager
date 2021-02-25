
#ifndef COLLECTION_H
#define COLLECTION_H

template <class T, int MAX_COLLECTION_SIZE>
class Collection
{
public:
    // начальные установки коллекции
    void begin(uint16_t size = MAX_COLLECTION_SIZE)
    {
        _size = (size < MAX_COLLECTION_SIZE) ? size : MAX_COLLECTION_SIZE;
        _index = 0;
    };
    // проверяет валидный ли индекс для коллекции
    bool isIndexValid(uint16_t index) { return ((index >= 0) && (index < _size)); };
    // возвращает текущий размер коллекции
    uint16_t getSize() { return _index; };
    // возвращает сохраненное значение по индексу
    T getItem(uint16_t index) { return (isIndexValid(index) ? _collection[index] : _collection[_size - 1]); }
    // возвращает ССЫЛКУ сохраненное значение по индексу
    T *get_item_link(uint16_t index) { return (isIndexValid(index) ? &_collection[index] : &_collection[_size - 1]); }
    // заменяет указанный элемент коллекцииж
    void setItem(uint16_t index, T item)
    {
        if (isIndexValid(index))
        {
            _collection[index] = item;
        }
        else
        {
            Serial.print(F("Index %d exceed the range\n")); //, index, _size - 1);
        }
    };
    // добавляет элемент в коллекцию
    void addItem(T item)
    {
        if ((_index) < _size)
        {
            _collection[_index] = item;
            _index++;
        }
        else
        {
            Serial.print(F("Collection exceed the range\n"));
        }
    };

private:
    uint16_t _index = 0;
    uint16_t _size = 1;
    T _collection[MAX_COLLECTION_SIZE];
};

#endif