import numpy as np

class HighLambdaHashing():
	def __init__(self, hash_input):
        #Total Number of unique elements
		num_elements = len(hash_input.keys())
		#Maximum value of the key, this determines the length of bitmap table and the count table
		self.max_key_value = max(hash_input.keys())
		#Initializing hash_entries table to all zero
		self.hash_entries = np.zeros(num_elements, dtype="U64")
		#intializing hash_entries_bitmap table to all zero
		self.hash_entries_bitmap = np.zeros(((int(np.ceil(self.max_key_value/8))),), dtype=np.uint8)
		#intializing hash_entries_count table to all zero
		self.hash_entries_count = np.zeros((int((np.ceil(self.max_key_value/8))),), dtype=np.uint16)

        #Start inserting all entries in sorted format
		for key in sorted(hash_input.keys()):
			self.insert(key, hash_input[key])

	def get_num_bit_set(self, bitmap, bit_location):
		count = 0
		for i in range(0, bit_location):
			count += ((bitmap & (1 << i)) >> i)
		return count

	def insert(self, key, value):
		coarse_offset = key // 8
		fine_offset = key % 8
		if (self.hash_entries_bitmap[coarse_offset] & (1 << fine_offset)):
			print("Entry Already_Present")
		else:
			self.hash_entries_bitmap[coarse_offset] = self.hash_entries_bitmap[coarse_offset] | (1 << fine_offset)
			i = coarse_offset + 1
			while i < int(np.ceil(self.max_key_value/8)):
				self.hash_entries_count[i] += 1
				i += 1
			hash_table_offset = self.hash_entries_count[coarse_offset] + self.get_num_bit_set(self.hash_entries_bitmap[coarse_offset], fine_offset)
			self.hash_entries[hash_table_offset] = value

	def display_hash_table(self):
		print((self.hash_entries))
		print(self.hash_entries_bitmap)
		print(self.hash_entries_count)