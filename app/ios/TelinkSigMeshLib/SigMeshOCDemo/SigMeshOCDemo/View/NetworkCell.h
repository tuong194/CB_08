/********************************************************************************************************
 * @file     NetworkCell.h
 *
 * @brief    A concise description.
 *
 * @author   Telink, 梁家誌
 * @date     2023/5/15
 *
 * @par     Copyright (c) 2023, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *
 *          Licensed under the Apache License, Version 2.0 (the "License");
 *          you may not use this file except in compliance with the License.
 *          You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 *          Unless required by applicable law or agreed to in writing, software
 *          distributed under the License is distributed on an "AS IS" BASIS,
 *          WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *          See the License for the specific language governing permissions and
 *          limitations under the License.
 *******************************************************************************************************/

#import "BaseCell.h"

NS_ASSUME_NONNULL_BEGIN

@interface NetworkCell : BaseCell
/// Background layer for setting rounded corners.
@property (weak, nonatomic) IBOutlet UIView *bgView;
/// Text layer used to set name.
@property (weak, nonatomic) IBOutlet UILabel *nameLabel;
/// Text layer used to set uuid.
@property (weak, nonatomic) IBOutlet UILabel *uuidLabel;
/// Text layer used to set create time.
@property (weak, nonatomic) IBOutlet UILabel *createTimeLabel;
/// Button layer used to set more icon image.
@property (weak, nonatomic) IBOutlet UIButton *moreButton;

#ifdef kIsTelinkCloudSigMeshLib
@property (nonatomic, strong) AppMeshNetworkModel *model;
#endif

@end

NS_ASSUME_NONNULL_END