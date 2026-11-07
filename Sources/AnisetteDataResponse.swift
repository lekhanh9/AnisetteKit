//
//  AnisetteDataResponse.swift
//  AnisetteKit
//
//  Created by Magesh K on 07/09/26.
//  Copyright © 2026 Magesh K. All rights reserved.
//

import Foundation

public struct AnisetteDataResponse: Sendable, Codable, Equatable, Hashable {
    public let oneTimePassword: String
    public let machineID: String
    public let routingInfo: String?
    public let clientInfo: String?
    public let userAgent: String?

    public init(
        oneTimePassword: String,
        machineID: String,
        routingInfo: String? = nil,
        clientInfo: String? = nil,
        userAgent: String? = nil
    ) {
        self.oneTimePassword = oneTimePassword
        self.machineID = machineID
        self.routingInfo = routingInfo
        self.clientInfo = clientInfo
        self.userAgent = userAgent
    }

    public init(from dictionary: [String: String]) throws {
        guard let otp = dictionary[AnisetteConstants.Headers.oneTimePassword],
              let mid = dictionary[AnisetteConstants.Headers.machineID] else 
        {
            throw AnisetteError.invalidResponse(reason: "Missing \(AnisetteConstants.Headers.oneTimePassword) or \(AnisetteConstants.Headers.machineID) from response")
        }
        self.oneTimePassword = otp
        self.machineID = mid
        self.routingInfo = dictionary[AnisetteConstants.Headers.routingInfo]
        self.clientInfo = dictionary[AnisetteConstants.Headers.clientInfo]
        self.userAgent = dictionary[AnisetteConstants.Headers.userAgent]
    }
}
