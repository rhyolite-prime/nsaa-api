//
// Created by Emmanuel Addo-Odame on 05/11/2026.
//

#ifndef CHOPMONEYRAFFLEAPI_CLOUDINARYAPI_H
#define CHOPMONEYRAFFLEAPI_CLOUDINARYAPI_H
#include <drogon/utils/coroutine.h>

#include <optional>
#include <string>

#include "dto/BaseApiResponse.h"

namespace raffle::services {


    class  CloudinaryApi
    {
    public:
        /**
         * @brief Resolves the mime type of a file from its file name.
         * @return The mime type, or an empty string when the extension is unknown.
         */
        static std::string mimeTypeForFile(const std::string &fileName);

        /**
         * @brief Uploads an in-memory file to Cloudinary.
         *
         * @param data        The raw bytes of the file.
         * @param length      The number of bytes in @p data.
         * @param fileName    Original file name, used to derive the file
         *                    extension reported to Cloudinary.
         * @param contentType Mime type of the file (e.g. "image/png"), may be
         *                    empty when unknown.
         * @param folder      Cloudinary folder the asset is stored in.
         *
         * @return The secure (https) delivery url of the uploaded asset, or an
         *         empty optional when the upload failed.
         */
        drogon::Task<std::optional<std::string>> uploadFile(
            const char *data,
            size_t length,
            const std::string &fileName,
            const std::string &contentType = "",
            const std::string &folder = "nsaa");

        /**
         * @brief Uploads a file that already lives on disk to Cloudinary.
         *
         * @return The secure (https) delivery url of the uploaded asset, or an
         *         empty optional when the upload failed.
         */
        drogon::Task<std::optional<std::string>> uploadFile(
            const std::string &filePath,
            const std::string &folder = "hero-slides");
    };


}
#endif //CHOPMONEYRAFFLEAPI_CLOUDINARYAPI_H
