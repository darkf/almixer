/*
 * Windows Media Foundation backend
 * Copyright (C) 2014 Eric Wing <ewing . public @ playcontrol.net>
 */
#ifdef _WIN32

#include "WindowsMediaFoundation_IMFByteStreamRWops.hpp"

//namespace ALmixer {

IMFByteStreamRWops::IMFByteStreamRWops(ALmixer_RWops* rw_ops, Sound_Sample* sound_sample)
	: 
	rwOps(rw_ops),
	soundSample(sound_sample),
	refCount(1)
{
}

IMFByteStreamRWops::~IMFByteStreamRWops()
{
}

STDMETHODIMP IMFByteStreamRWops::QueryInterface(REFIID riid, LPVOID * ppvObj)
{
	// Always set out parameter to NULL, validating it first.
	if (!ppvObj)
	{
		return E_INVALIDARG;
	}
	*ppvObj = NULL;
	if (riid == IID_IUnknown 
		|| riid == IID_IMFByteStream)
	{
		// Increment the reference count and return the pointer.
		*ppvObj = (LPVOID)this;
		AddRef();
		return NOERROR;
	}
	return E_NOINTERFACE;
}

ULONG IMFByteStreamRWops::AddRef()
{
	InterlockedIncrement(&refCount);
	return refCount;
}

ULONG IMFByteStreamRWops::Release()
{
	// Decrement the object's internal counter.
	ULONG ulRefCount = InterlockedDecrement(&refCount);
	if (0 == refCount)
	{
		delete this;
	}
	return ulRefCount;
}

// IMFByteStream Methods
STDMETHODIMP IMFByteStreamRWops::BeginRead(BYTE* data_buffer, ULONG buffer_length, IMFAsyncCallback* async_callback, IUnknown* user_data)
{
//	SNDDBG(("WindowsMediaFoundation IMFByteStreamRWops: BeginRead()\n"));


	/*
	I don't really understand what I'm really supposed to be doing.
	http://msdn.microsoft.com/en-us/library/windows/desktop/ms703187%28v=vs.85%29.aspx
	It seems that I am expected to make some object which ties in with an AsyncCallback.
	But in the above example, they create the actual AsyncCallback, whereas, I am getting a pre-canned one passed to me.
	So I don't know the correct way of making sure my tied in object does what it needs to do.
	I think this whole thing is overly complicated and I don't really want this to be asynchronous.
	ALmixer already has a threading model and I don't need this complicating things even more.
	So it seems I need my helper object so I can pass data to the EndRead() function (number of bytes read),
	so I can't escaping creating that object.
	It also seems that I must call MFInvokeCallback or EndRead() will never get called.
	But I've decided to call Read() directly in here. I suspect I' expected to do this somewhere else and related to async,
	but this works and is simple.
	*/

	AsyncReadRWopsHelper* read_helper = new AsyncReadRWopsHelper(rwOps, data_buffer, buffer_length);

	HRESULT hr = MFCreateAsyncResult(read_helper, async_callback, user_data, &currentReadRWopsAsyncResult);
	read_helper->Release();
	if (FAILED(hr))
	{
		return hr;
	}

	ULONG bytes_read = 0;
	Read(data_buffer, buffer_length, &bytes_read);
	read_helper->setBytesRead(bytes_read);

	hr = MFInvokeCallback(currentReadRWopsAsyncResult);
//	SNDDBG(("WindowsMediaFoundation IMFByteStreamRWops: returning from BeginRead()\n"));

	return S_OK;
}

STDMETHODIMP
IMFByteStreamRWops::BeginWrite(const BYTE* data_buffer, ULONG buffer_length, IMFAsyncCallback* async_callback, IUnknown* user_data)
{
//	SNDDBG(("WindowsMediaFoundation IMFByteStreamRWops: BeginWrite()"));
	return E_NOTIMPL;
}
STDMETHODIMP
IMFByteStreamRWops::Close()
{
	// SoundDecoder handles this at a higher level.
//	SNDDBG(("WindowsMediaFoundation IMFByteStreamRWops: Close()"));
	return S_OK;
}

STDMETHODIMP
IMFByteStreamRWops::EndRead(IMFAsyncResult* async_result, ULONG* out_bytes_read)
{
//	SNDDBG(("WindowsMediaFoundation IMFByteStreamRWops: EndRead()\n"));

	if(!out_bytes_read)
	{
		return E_INVALIDARG;
	}
	IUnknown* user_data;
	async_result->GetObject(&user_data);

	AsyncReadRWopsHelper* read_helper = reinterpret_cast<AsyncReadRWopsHelper*>(user_data);
	*out_bytes_read = read_helper->getBytesRead();
	user_data->Release();

	// Need to free the object which was created in BeginRead
	currentReadRWopsAsyncResult->Release();
	currentReadRWopsAsyncResult = NULL;
//	SNDDBG(("WindowsMediaFoundation IMFByteStreamRWops: returning from EndRead()\n\n"));

	return S_OK;
}

STDMETHODIMP
IMFByteStreamRWops::EndWrite(IMFAsyncResult* async_result, ULONG* out_bytes_written)
{
//	SNDDBG(("WindowsMediaFoundation IMFByteStreamRWops: EndWrite()"));
	return E_NOTIMPL;
}

STDMETHODIMP
IMFByteStreamRWops::Flush()
{
//	SNDDBG(("WindowsMediaFoundation IMFByteStreamRWops: Flush()"));
	return S_OK;
}

STDMETHODIMP
IMFByteStreamRWops::GetCapabilities(DWORD* out_capabilities)
{
 // SNDDBG(("WindowsMediaFoundation IMFByteStreamRWops: GetCapabilities()"));

	*out_capabilities = MFBYTESTREAM_IS_READABLE
		| MFBYTESTREAM_IS_SEEKABLE
#if (WINVER >= _WIN32_WINNT_WIN8)
		| MFBYTESTREAM_DOES_NOT_USE_NETWORK
#else
	// If you don't have the Win8 headers, this is the constant. 
	// I believe the flag will be ignored by the runtime on pre-Win 8.
	| 0x00000800
#endif

	;
	return S_OK;
}

STDMETHODIMP
IMFByteStreamRWops::GetCurrentPosition(QWORD* a_position)
{
//	SNDDBG(("WindowsMediaFoundation IMFByteStreamRWops: GetCurrentPosition()"));
	*a_position = (QWORD)ALmixer_RWtell(rwOps);
	return S_OK;
}

STDMETHODIMP
IMFByteStreamRWops::GetLength(QWORD* a_length)
{
//  SNDDBG(("WindowsMediaFoundation IMFByteStreamRWops: GetLength()"));

	INT64 current_position = ALmixer_RWtell(rwOps);
	INT64 end_position = ALmixer_RWseek(rwOps, 0, SEEK_END);
	ALmixer_RWseek(rwOps, current_position, SEEK_SET);

	*a_length = (QWORD)end_position;
	return S_OK;
}


STDMETHODIMP
IMFByteStreamRWops::IsEndOfStream(BOOL* end_of_stream)
{
	if(soundSample->flags & SOUND_SAMPLEFLAG_EOF)
	{
		*end_of_stream = true;
	}
	else
	{
		*end_of_stream = false;
	}
	return S_OK;
}

STDMETHODIMP
IMFByteStreamRWops::Read(BYTE* data_buffer, ULONG buffer_length, ULONG* out_bytes_read)
{
//	SNDDBG(("WindowsMediaFoundation IMFByteStreamRWops: Read()\n"));

	*out_bytes_read = ALmixer_RWread(rwOps, data_buffer, 1, buffer_length);
	return S_OK;
}

STDMETHODIMP
IMFByteStreamRWops::Seek(MFBYTESTREAM_SEEK_ORIGIN seek_origin, LONGLONG seek_offset, DWORD seek_flags, QWORD* out_current_position)
{
//	SNDDBG(("WindowsMediaFoundation IMFByteStreamRWops: Seek()"));
	INT64 new_position;
	if(msoBegin == seek_origin)
	{
		SNDDBG2("WindowsMediaFoundation IMFByteStreamRWops: Seek() msoBegin");
		new_position = seek_offset;
	}
	else
	{
		SNDDBG2("WindowsMediaFoundation IMFByteStreamRWops: Seek() msoCurrent");
		INT64 current_position = ALmixer_RWtell(rwOps);
		new_position = seek_offset + current_position;
	}
	ALmixer_RWseek(rwOps, new_position, SEEK_SET);
	*out_current_position = new_position;
	return S_OK;
}

STDMETHODIMP
IMFByteStreamRWops::SetCurrentPosition(QWORD a_position)
{
//	SNDDBG(("WindowsMediaFoundation IMFByteStreamRWops: SetCurrentPosition()"));
	ALmixer_RWseek(rwOps, a_position, SEEK_SET);

	return S_OK;
}
STDMETHODIMP
IMFByteStreamRWops::SetLength(QWORD a_length)
{
//	SNDDBG(("WindowsMediaFoundation IMFByteStreamRWops: SetLength()"));
	return E_NOTIMPL;
}

STDMETHODIMP
IMFByteStreamRWops::Write(const BYTE* data_buffer, ULONG buffer_length, ULONG* out_bytes_written)
{
//	SNDDBG(("WindowsMediaFoundation IMFByteStreamRWops: Write()"));
	return E_NOTIMPL;
}






/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// AsyncReadRWopsHelper
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

STDMETHODIMP AsyncReadRWopsHelper::QueryInterface(REFIID riid, LPVOID * ppvObj)
{
	// Always set out parameter to NULL, validating it first.
	if (!ppvObj)
	{
		return E_INVALIDARG;
	}
	*ppvObj = NULL;
	if (riid == IID_IUnknown)
	{
		// Increment the reference count and return the pointer.
		*ppvObj = (LPVOID)this;
		AddRef();
		return NOERROR;
	}
	return E_NOINTERFACE;
}

ULONG AsyncReadRWopsHelper::AddRef()
{
	InterlockedIncrement(&refCount);
	return refCount;
}

ULONG AsyncReadRWopsHelper::Release()
{
	// Decrement the object's internal counter.
	ULONG ulRefCount = InterlockedDecrement(&refCount);
	if (0 == refCount)
	{
		delete this;
	}
	return ulRefCount;
}




// } // namespace ALmixer

#endif /* _WIN32 */

