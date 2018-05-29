#ifndef LANG_HUFFMAN16_H
#define LANG_HUFFMAN16_H


#include <lang/Array.h>
#include <stdint.h>


BEGIN_NAMESPACE(lang) 


/** 
 * 16bit Huffman-compression helper class.
 * @ingroup lang
 */
class Huffman16
{
public:
	/* Huffman tree node. Implementation use only. */
	class HuffmanNode16
	{
	public:
		HuffmanNode16*	left;
		HuffmanNode16*	right;
		HuffmanNode16*	parent;
		int				value;
		int				code;
		int				count;
		int				level;
		int				depth;
		int				sortvalue;
		bool			ignore;

		HuffmanNode16();
		HuffmanNode16( HuffmanNode16* left0, HuffmanNode16* right0 );
	};

	/* Huffman tree node sorter. Uses sortvalue field to sort items in non-descending order. Implemetatino use only. */
	class HuffmanNodeSorter16
	{
	public:
		bool operator()( HuffmanNode16* a, HuffmanNode16* b ) const		{return a->sortvalue < b->sortvalue;}
	};

	Huffman16();

	~Huffman16();

	/**
	 * Creates Huffman tree from specified sample data.
	 */
	void	buildTree( const uint16_t* data, int count );

	/**
	 * Writes tree to user Array.
	 * Clears Array before writing.
	 */
	void	writeTree( Array<uint8_t>* treedata );

	/**
	 * Reads tree from user Array.
	 * @return Number of bytes read
	 */
	int		readTree( const uint8_t* treedata, int treedatasize );

	/**
	 * Builds Huffman-tree, writes the tree, data size and compressed data to user Array.
	 */
	void	compress( const uint16_t* data, int count, Array<uint8_t>* out );

	/**
		 * Reads Huffman-tree, data size and compressed data from the array.
	 */
	void	decompress( const uint8_t* data, int count, Array<uint16_t>* out );

	/**
	 * Compresses specified data block and writes output to user Array.
	 * Does not clear Array before writing the output.
	 * @param bytecount Number of bytes to encode.
	 * @return true if data was succesfully compressed using this Huffman-tree, false if some symbols missing.
	 */
	bool	encode( const uint16_t* data, int bytecount, Array<uint8_t>* out );

	/**
	 * Decompresses specified Huffman-coded data block and writes output to user Array.
	 * Does not clear Array before writing the output.
	 * @param bitcount Number of BITS to decode.
	 * @param firstbit First bit index to decode.
	 */
	void	decode( const uint8_t* data, int bitcount, Array<uint16_t>* out, int firstbit=0 );

private:
	enum { MAX_LEVELS = 16 };

	Array<HuffmanNode16*>	m_huffmanArray;
	HuffmanNode16*			m_huffmanTree;
	Array<int>				m_startCodes;
	Array<int>				m_startIndices;
	Array<int>				m_levelCounts;
	Array<HuffmanNode16*>	m_encodingTable;
	Array<HuffmanNode16>	m_allocNodes;
	Array<HuffmanNode16*>	m_allocNodesPtr;

	HuffmanNode16*			buildTree( HuffmanNode16** nodes, int count );
	void					assignCodes( Array<Huffman16::HuffmanNode16*>& huffmanarray );
	void					deleteTree();

	static int				findMinCountNode( HuffmanNode16** nodes, int count );
	static void				printTree( HuffmanNode16* node );
	static void				increaseLevel( HuffmanNode16* node );
	static void				getBitString( int x, int bitcount, char* buf );
	static void				getLeafNodes( Huffman16::HuffmanNode16* node, Array<Huffman16::HuffmanNode16*>& huffmanarray );
	static void				getLeafNodesR( Huffman16::HuffmanNode16* node, Array<Huffman16::HuffmanNode16*>& huffmanarray );
	static int				getHuffmanCode( Huffman16::HuffmanNode16* node );
	static void				assignNonCanonicalCodes( Huffman16::HuffmanNode16* node );
	static int				getUniqueLeafLevelCount( const Array<Huffman16::HuffmanNode16*>& a );
	static void				getLinearTableByValue( const Array<Huffman16::HuffmanNode16*>& src, Array<Huffman16::HuffmanNode16*>& linear );
	static void				getLinearTableByCode( const Array<Huffman16::HuffmanNode16*>& src, Array<Huffman16::HuffmanNode16*>& linear );
	static int				getLevelCounts( Array<Huffman16::HuffmanNode16*>& huffmanarray, Array<int>& levelcounts );
	static void				writeUInt8( Array<uint8_t>* out, int x )		{assert( x >= 0 && x < 0x100 ); out->add( (uint8_t)x );}
	static void				writeUInt16( Array<uint8_t>* out, int x )		{assert( x >= 0 && x < 0x10000 ); out->add( (uint8_t)x ); out->add( (uint8_t)(x>>8) );}
	static void				writeInt32( Array<uint8_t>* out, int x )		{out->add( (uint8_t)x ); out->add( (uint8_t)(x>>8) ); out->add( (uint8_t)(x>>16) ); out->add( (uint8_t)(x>>24) );}
	static int				readUInt8( const uint8_t*& readptr )			{return *readptr++;}
	static int				readUInt16( const uint8_t*& readptr )			{int x = int(readptr[0]) + (int(readptr[1])<<8); readptr += 2; return x;}
	static int				readInt32( const uint8_t*& readptr )			{int x = int(readptr[0]) + (int(readptr[1])<<8) + (int(readptr[2])<<16) + (int(readptr[3])<<24); readptr += 4; return x;}
	static void				printCodes( const Array<Huffman16::HuffmanNode16*>& huffmanarray );
	static void				updateDepth( Huffman16::HuffmanNode16* node, int level=0 );
	static int				getDepth( Huffman16::HuffmanNode16* node );
	static int				reverseBits( int x, int bitcount );
	static void				checkTree( Huffman16::HuffmanNode16* node );
};


END_NAMESPACE() // lang


#endif // LANG_HUFFMAN16_H
