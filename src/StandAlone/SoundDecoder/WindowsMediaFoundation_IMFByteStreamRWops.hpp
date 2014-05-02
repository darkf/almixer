/*
 * Windows Media Foundation backend
 * Copyright (C) 2014 Eric Wing <ewing . public @ playcontrol.net>
 */
#ifdef _WIN32

#ifndef WindowsMediaFoundation_IMFByteStreamRWops_h
#define WindowsMediaFoundation_IMFByteStreamRWops_h

#include <windows.h>
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include "SoundDecoder.h"
#include "SoundDecoder_Internal.h"

// namespace ALmixer {

class IMFByteStreamRWops : public IMFByteStream, public IUnknown
{
public:
	IMFByteStreamRWops(ALmixer_RWops* rw_ops, Sound_Sample* sound_sample);
	~IMFByteStreamRWops();

	// IUnknown Methods.
	STDMETHODIMP QueryInterface(REFIID aIId, LPVOID *aInterface);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();

  // IMFByteStream Methods.
	STDMETHODIMP BeginRead(BYTE* data_buffer, ULONG buffer_length, IMFAsyncCallback* async_callback, IUnknown* user_data);
	STDMETHODIMP BeginWrite(const BYTE* data_buffer, ULONG buffer_length, IMFAsyncCallback* async_callback, IUnknown* user_data);
	STDMETHODIMP Close();
	STDMETHODIMP EndRead(IMFAsyncResult* async_result, ULONG* out_bytes_read);
	STDMETHODIMP EndWrite(IMFAsyncResult* async_result, ULONG* out_bytes_written);
	STDMETHODIMP Flush();
	STDMETHODIMP GetCapabilities(DWORD* out_capabilities);
	STDMETHODIMP GetCurrentPosition(QWORD* a_position);
	STDMETHODIMP GetLength(QWORD *a_length);
	STDMETHODIMP IsEndOfStream(BOOL *end_of_stream);
	STDMETHODIMP Read(BYTE* data_buffer, ULONG buffer_length, ULONG* out_bytes_read);
	STDMETHODIMP Seek(MFBYTESTREAM_SEEK_ORIGIN seek_origin, LONGLONG seek_offset, DWORD seek_flags, QWORD* out_current_position);
	STDMETHODIMP SetCurrentPosition(QWORD a_position);
	STDMETHODIMP SetLength(QWORD a_length);
	STDMETHODIMP Write(const BYTE* data_buffer, ULONG buffer_length, ULONG* out_bytes_written);


protected:
	ALmixer_RWops* rwOps;
	Sound_Sample* soundSample;
	LONG refCount;
	IMFAsyncResult* currentReadRWopsAsyncResult;

};


// AsyncReadState is a helper class used in BeginRead.
class AsyncReadRWopsHelper : public IUnknown
{
public:
	AsyncReadRWopsHelper(ALmixer_RWops* rw_ops, BYTE* data_buffer, ULONG buffer_length)
		:
		refCount(1),
		rwOps(rw_ops),
		dataBuffer(data_buffer),
		bufferLength(buffer_length),
		bytesRead(0)
	{
	}
	// IUnknown Methods.
	STDMETHODIMP QueryInterface(REFIID aIId, LPVOID *aInterface);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();

	BYTE* getDataBuffer() const { return dataBuffer; }
	ULONG getBufferLength() const { return bufferLength; }
	ULONG getBytesRead() const { return bytesRead; }
	void setBytesRead(ULONG bytes_read) { bytesRead = bytes_read; }

protected:
	long refCount;
	ALmixer_RWops* rwOps;
	BYTE* dataBuffer;
	ULONG bufferLength;
	ULONG bytesRead;
};


// } // namespace ALmixer

#endif

#endif /* _WIN32 */

