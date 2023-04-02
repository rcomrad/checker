#include "post_handler.hpp"

#include "domain/path.hpp"

#include "database/connection_manager.hpp"

#include "post_router.hpp"

std::string
post::PostHandler::uploadFile(crow::multipart::message& aMsg,
                              std::string aPathPrefix)
{
    if (aPathPrefix.empty()) aPathPrefix = dom::Path::getPath("upload").value();

    // TODO: filename
    auto fileName = aMsg.get_part_by_name("filename").body;
    auto file     = aMsg.get_part_by_name("file").body;

    std::string filePath;
    {
        auto connection = data::ConnectionManager::getUserConnection();
        auto table      = connection.val.getData<data::File>();
        filePath =
            aPathPrefix + std::to_string(table[0].num++) + "-" + fileName;
        connection.val.update<data::File>(table);
    }

    auto file_handler = std::ofstream(filePath);
    file_handler << file;
    file_handler.close();

    return filePath;
}

void
post::PostHandler::transmitToMTMHandler(const std::string aTableName,
                                        int aID,
                                        bool aIsAdding,
                                        std::vector<int> aIDForInsert)
{
    PostRouter::manyToManyRouter(aTableName, aID, aIsAdding, aIDForInsert);
}
