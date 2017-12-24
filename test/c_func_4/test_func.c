int my_abs(int value) {
	if (value >= 0) {
		return value;
	} else {
		return -value;
	}
}

int main(void) {
	int value = -3;
	return my_abs(value);
}
