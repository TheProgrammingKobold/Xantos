#pragma once

#include "RenderAssetTypes.h"
#include "../Render/Vertex.h"

#include <glad/glad.h>

#include <atomic>
#include <cstdint>
#include <deque>
#include <mutex>
#include <utility>
#include <vector>

struct MeshUploadRequest
{
    uint64_t requestID = 0;

    int chunkX = 0;
    int chunkZ = 0;

    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
};

struct MeshUploadResult
{
    uint64_t requestID = 0;

    int chunkX = 0;
    int chunkZ = 0;

    MeshID mesh;
};

struct MeshReleaseRequest
{
    MeshID mesh;

    // The renderer must have rendered a packet
// with at least this frame number before the
// mesh may be destroyed.
    uint64_t safeAfterFrame = 0;
};

class RenderResourceQueue
{
public:

    uint64_t NextRequestID()
    {
        return _nextRequestID.fetch_add(
            1,
            std::memory_order_relaxed
        );
    }

    void SubmitMeshUpload(
        MeshUploadRequest request
    )
    {
        std::lock_guard lock(_mutex);

        _meshUploads.push_back(
            std::move(request)
        );
    }

    bool TryGetMeshUpload(
        MeshUploadRequest& request
    )
    {
        std::lock_guard lock(_mutex);

        if (_meshUploads.empty())
            return false;

        request =
            std::move(_meshUploads.front());

        _meshUploads.pop_front();

        return true;
    }

    void SubmitMeshUploadResult(
        MeshUploadResult result
    )
    {
        std::lock_guard lock(_mutex);

        _meshUploadResults.push_back(
            std::move(result)
        );
    }

    bool TryGetMeshUploadResult(
        MeshUploadResult& result
    )
    {
        std::lock_guard lock(_mutex);

        if (_meshUploadResults.empty())
            return false;

        result =
            std::move(_meshUploadResults.front());

        _meshUploadResults.pop_front();

        return true;
    }

    void SubmitMeshRelease(
        MeshReleaseRequest request
    )
    {
        if (!request.mesh)
            return;

        std::lock_guard lock(_mutex);

        _meshReleases.push_back(request);
    }

    bool TryGetMeshRelease(
        MeshReleaseRequest& request
    )
    {
        std::lock_guard lock(_mutex);

        if (_meshReleases.empty())
            return false;

        request = _meshReleases.front();

        _meshReleases.pop_front();

        return true;
    }

private:

    std::mutex _mutex;

    std::deque<MeshUploadRequest>
        _meshUploads;

    std::deque<MeshUploadResult>
        _meshUploadResults;

    std::deque<MeshReleaseRequest>
        _meshReleases;

    std::atomic<uint64_t>
        _nextRequestID{ 1 };
};