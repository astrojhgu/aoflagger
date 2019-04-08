#ifndef ARRAYCOLUMNITERATOR_H
#define ARRAYCOLUMNITERATOR_H

#include <casacore/tables/Tables/ArrayColumn.h>
#include <casacore/tables/Tables/RefRows.h>

template<typename T>
class ArrayColumnIterator {
public:
	ArrayColumnIterator(casacore::ArrayColumn<T> &column, unsigned row) noexcept :
		_column(column), _row(row)
	{
	}
	
	ArrayColumnIterator(const ArrayColumnIterator<T> &source) noexcept :
		_column(source._column), _row(source._row)
	{
	}
	
	ArrayColumnIterator<T> &operator++() noexcept {
		_row++;
		return *this;
	}
	
	casacore::Array<T> *operator->() const noexcept {
		return &_column(_row);
	}
	
	casacore::Array<T> operator*() const noexcept {
		return _column(_row);
	}
	
	bool operator!=(const ArrayColumnIterator<T> &other) const noexcept {
		return _row!=other._row;
	}
	
	bool operator==(const ArrayColumnIterator<T> &other) const noexcept {
		return _row==other._row;
	}
	
	void Set(const casacore::Array<T>& values) noexcept
	{
		_column->basePut(_row, values);
	}
	
	static ArrayColumnIterator First(casacore::ArrayColumn<T> &column)
	{
		return ArrayColumnIterator<T>(column, 0);
	}
	
private:
	casacore::ArrayColumn<T>& _column;
	unsigned _row;
};

#endif
